#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

// Frame Buffer
// -------------
// If you press jump while the player is in the air (a few frames before
// landing), the press is remembered. As soon as the player touches the
// ground within the buffer window, the jump is fired automatically.
//
// Works in every gamemode. Orbs/pads are left to the game's own handling.

class $modify(FrameBufferLayer, GJBaseGameLayer) {
    struct Fields {
        bool pending[2]     = {false, false}; // a buffered click is waiting
        int  framesLeft[2]  = {0, 0};         // frames before it expires
        bool held[2]        = {false, false}; // user is physically holding
        bool releaseNext[2] = {false, false}; // release injected click next step
    };

    static bool bufferEnabled() {
        return Mod::get()->getSettingValue<bool>("enabled");
    }

    static int bufferFrames() {
        return static_cast<int>(Mod::get()->getSettingValue<int64_t>("buffer-frames"));
    }

    PlayerObject* playerAt(int i) {
        return i == 0 ? m_player1 : m_player2;
    }

    void injectJump(bool down, int i) {
        // Qualified call goes straight to the original game function,
        // so it doesn't touch our own `held` bookkeeping.
        GJBaseGameLayer::handleButton(down, 1, i == 0);
    }

    void handleButton(bool down, int button, bool isPlayer1) {
        // button 1 == jump
        if (button == 1 && bufferEnabled()) {
            auto f = m_fields.self();
            int i = isPlayer1 ? 0 : 1;
            auto player = playerAt(i);

            if (down) {
                f->held[i] = true;
                if (player && !player->m_isOnGround) {
                    f->pending[i] = true;
                    f->framesLeft[i] = bufferFrames();
                }
            } else {
                f->held[i] = false;
            }
        }
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }

    void tickBuffer() {
        auto f = m_fields.self();
        for (int i = 0; i < 2; i++) {
            auto player = playerAt(i);
            if (!player) {
                f->pending[i] = false;
                f->releaseNext[i] = false;
                continue;
            }

            // Release the click we injected on the previous step.
            if (f->releaseNext[i]) {
                f->releaseNext[i] = false;
                if (!f->held[i]) this->injectJump(false, i);
            }

            if (!f->pending[i]) continue;

            if (player->m_isOnGround) {
                // Jump is possible now: fire the buffered click.
                f->pending[i] = false;
                this->injectJump(true, i);
                if (!f->held[i]) f->releaseNext[i] = true;
            } else if (--f->framesLeft[i] <= 0) {
                f->pending[i] = false; // too early, buffer expired
            }
        }
    }

    // Runs once per physics step, so the buffer is measured in real
    // game frames/steps (not render frames).
    void processCommands(float dt) {
        if (bufferEnabled()) this->tickBuffer();
        GJBaseGameLayer::processCommands(dt);
    }
};
