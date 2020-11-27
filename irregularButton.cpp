#include "irregularButton.h"

irregularButton::irregularButton() :
    Button(),
    _imageAlpha(nullptr),
    _isOptimized(false),
    _alphaCheckValue(0)
{}

irregularButton::irregularButton(int alphaCheckValue, bool isOptimized) :
    Button(),
    _imageAlpha(nullptr),
    _isOptimized(isOptimized),
    _alphaCheckValue(alphaCheckValue) {}

irregularButton::~irregularButton() {
    delete[] _imageAlpha;
}

irregularButton* irregularButton::create(const std::string& normalImage, const std::string& selectedImage, const std::string& disableImage ,int alphaCheckValue, bool isOptimized, Widget::TextureResType texType) {
    irregularButton* ir_btn = new irregularButton(isOptimized, alphaCheckValue);
    if (ir_btn && ir_btn->init(normalImage, selectedImage, disableImage, texType)) {
        ir_btn->autorelease();
        return ir_btn;
    }
    CC_SAFE_DELETE(ir_btn);
    return nullptr;
}

irregularButton* irregularButton::createWithButton(Button * btn, int alphaCheckValue, bool isOptimized) {
    string normalImage = btn->getNormalFile().file;
    string selectedImage = btn->getPressedFile().file;
    string disableImage = btn->getDisabledFile().file;
    TextureResType res_type = (TextureResType)btn->getNormalFile().type;
    irregularButton* ir_btn = new irregularButton(isOptimized, alphaCheckValue);
    if (ir_btn && ir_btn->init(normalImage, selectedImage, disableImage, res_type)) {
        ir_btn->autorelease();
        return ir_btn;
    }
    CC_SAFE_DELETE(ir_btn);
    return nullptr;
}

bool irregularButton::init(const std::string &normalImage, const std::string& selectedImage, const std::string& disableImage, TextureResType texType) {
    bool ret = true;
    do {
        if (!Button::init(normalImage, selectedImage, disableImage, texType)) {
            ret = false;
            break;
        }
    } while (0);
    loadNormalTransparentInfo(normalImage);
    return ret;
}

void irregularButton::loadNormalTransparentInfo(std::string sName) {
    Image* normalImage = new Image();
    normalImage->initWithImageFile(sName);
    normalImageWidth_ = normalImage->getWidth();
    normalImageHeight_ = normalImage->getHeight();
    this->setContentSize(Size(normalImageWidth_, normalImageHeight_));
    if (_imageAlpha != nullptr) {
        delete[] _imageAlpha;
    }
    unsigned char* imageData = normalImage->getData();

    _alphaDataWidth = normalImageWidth_;//default
    _alphaDataHeight = normalImageHeight_;

    if (_isOptimized) {
        _alphaDataWidth = (normalImageWidth_ + 7) >> 3;//char == 8 bit,
        _alphaDataLength = _alphaDataWidth * normalImageHeight_ * sizeof(unsigned char);
        _imageAlpha = new unsigned char[_alphaDataLength];
        memset(_imageAlpha, 0, _alphaDataLength);
        for (int i = 0; i < normalImageHeight_; i++)
        {
            for (int j = 0; j < normalImageWidth_; j += 8)
            {
                int aj = j >> 3;
                for (int k = 0; k < 8; k++)
                {
                    if (j + k >= normalImageWidth_)
                    {
                        break;
                    }
                    unsigned char alpha = imageData[((i*normalImageWidth_ + j + k) << 2) + 3];
                    if (alpha > _alphaCheckValue)
                    {//set 1 in the corresponding bit position
                        int index = i * _alphaDataWidth + aj;
                        _imageAlpha[index] = _imageAlpha[index] | (1 << k);
                    }
                }
            }
        }
    }
    else {
        _alphaDataLength = normalImageWidth_ * normalImageHeight_ * sizeof(unsigned char);
        _imageAlpha = new unsigned char[_alphaDataLength];
        for (int i = 0; i < normalImageHeight_; i++)
        {
            for (int j = 0; j < normalImageWidth_; j++)
            {
                int index = i * normalImageWidth_ + j;
                _imageAlpha[index] = imageData[(index << 2) + 3];
            }
        }
    }
    delete normalImage;
}

bool irregularButton::getIsTransparentAtPoint(cocos2d::Vec2 point) const {
    auto data = _imageAlpha;
    if (data == nullptr) {
        return true;
    }
    bool isTouchClaimed = false;
    auto locationInNode = point;//this->convertToNodeSpace(point);
    int x = (int)locationInNode.x;
    int y = (int)locationInNode.y;
    if (x >= 0 && x < normalImageWidth_ && y >= 0 && y < normalImageHeight_) {
        if (_isOptimized) {
            unsigned int i = (_alphaDataHeight - y - 1) * _alphaDataWidth + (x >> 3);
            unsigned char mask = 1 << (x & 0x07);
            if (i < _alphaDataLength && ((data[i] & mask) != 0))
            {
                isTouchClaimed = true;
            }
        }
        else
        {
            unsigned int i = (_alphaDataHeight - y - 1) * _alphaDataWidth + x;
            if (i < _alphaDataLength)
            {
                if ((unsigned int)data[i] > _alphaCheckValue)
                {
                    isTouchClaimed = true;
                }
            }
        }
    }
    return isTouchClaimed;
}

bool irregularButton::hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const {
    Vec2 localLocation = _buttonNormalRenderer->convertToNodeSpace(pt);
    Rect validTouchedRect;
    validTouchedRect.size = _buttonNormalRenderer->getContentSize();
    if (validTouchedRect.containsPoint(localLocation) && getIsTransparentAtPoint(localLocation))
    {
        //NotificationCenter::getInstance()->postNotification("NotifyIrregularBtn", (Ref*)m_iBtnID);
        return true;
    }
   return false;
}