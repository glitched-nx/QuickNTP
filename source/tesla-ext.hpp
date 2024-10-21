#include <tesla.hpp>

#include <string>
#include <vector>

namespace tsl::elm {
class CustomDrawerUnscissored : public Element {
public:
    CustomDrawerUnscissored(std::function<void(gfx::Renderer* r, s32 x, s32 y, s32 w, s32 h)> renderFunc) : Element(), m_renderFunc(renderFunc) {}
    virtual ~CustomDrawerUnscissored() {}

    virtual void draw(gfx::Renderer* renderer) override {
        this->m_renderFunc(renderer, ELEMENT_BOUNDS(this));
    }

    virtual void layout(u16 parentX, u16 parentY, u16 parentWidth, u16 parentHeight) override {
    }

private:
    std::function<void(gfx::Renderer*, s32 x, s32 y, s32 w, s32 h)> m_renderFunc;
};

class CustomOverlayFrame : public OverlayFrame {
public:
    CustomOverlayFrame(const std::string& title, const std::string& subtitle) : OverlayFrame(title, subtitle) {}

    virtual void layout(u16 parentX, u16 parentY, u16 parentWidth, u16 parentHeight) override {
        this->setBoundaries(parentX, parentY, parentWidth, parentHeight);

        if (this->m_contentElement != nullptr) {
            this->m_contentElement->setBoundaries(parentX + 35, parentY + 95, parentWidth - 85, parentHeight - 73 - 95);
            this->m_contentElement->invalidate();
        }
    }
};

class NamedStepTrackBarVector : public StepTrackBar {
public:
    /**
     * @brief Constructor
     *
     * @param icon Icon shown next to the track bar
     * @param stepDescriptions Step names displayed above the track bar
     */
    NamedStepTrackBarVector(const char icon[3], std::vector<std::string> stepDescriptions)
        : StepTrackBar(icon, stepDescriptions.size(), true), m_stepDescriptions(stepDescriptions.begin(), stepDescriptions.end()) {}

    virtual ~NamedStepTrackBarVector() {}

    virtual void draw(gfx::Renderer* renderer) override {

        // TrackBarV2 width excluding the handle areas
        u16 trackBarWidth = this->getWidth() - 95;
        
        // Base X and Y coordinates
        u16 baseX = this->getX() + 59;
        u16 baseY = this->getY() + 44; // 50 - 3
        
        s32 iconOffset;
        
        if (m_icon[0] != '\0') {
            s32 iconWidth = 23;//renderer->calculateStringWidth(m_icon, 23);
            iconOffset = 14 + iconWidth;
            baseX += iconOffset;
            trackBarWidth -= iconOffset;
        }
        // Calculate the spacing between each step
        float stepSpacing = static_cast<float>(trackBarWidth) / (this->m_numSteps - 1);
        
        // Calculate the halfway point index
        u8 halfNumSteps = (this->m_numSteps - 1) / 2;
        // Draw step rectangles
        //u16 stepX;
        for (u8 i = 0; i < this->m_numSteps; i++) {
            u16 stepX = baseX + std::round(i * stepSpacing);
            
            // Subtract 1 from the X position for steps on the right side of the center
            if (i > halfNumSteps) {
                stepX -= 1;
            }
            // Adjust the last step to avoid overshooting
            if (i == this->m_numSteps - 1) {
                stepX = baseX + trackBarWidth -1;
            }
    
            renderer->drawRect(stepX, baseY, 1, 8, a(trackBarEmptyColor));
        }
        
        u8 currentDescIndex = std::clamp(this->m_value / (100 / (this->m_numSteps - 1)), 0, this->m_numSteps - 1);
        auto descWidth = tsl::gfx::calculateStringWidth(this->m_stepDescriptions[currentDescIndex].c_str(), 15);
        renderer->drawString(this->m_stepDescriptions[currentDescIndex].c_str(), false, ((baseX +1) + (trackBarWidth) / 2) - (descWidth / 2), this->getY() + 20 + 6, 15, a(tsl::style::color::ColorDescription));
        
        // Draw the parent trackbar
        StepTrackBar::draw(renderer);
    }

protected:
    std::vector<std::string> m_stepDescriptions;
};
} // namespace tsl::elm
