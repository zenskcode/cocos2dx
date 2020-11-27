#ifndef __irregularButton_h__
#define __irregularButton_h__ 

#include "header.h"

class irregularButton : public Button {
public:
    irregularButton();
    irregularButton(int alphaCheckValue, bool isOptimized);
    virtual ~irregularButton();

    virtual bool init(const std::string& normalImage, const std::string& selectedImage = "", const std::string& disableImage = "", Widget::TextureResType texType = Widget::TextureResType::LOCAL) override;
    virtual bool hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const override;
    
    /*
        @param alphaCheckValue          if image's alphaValue larger than alphaCheckValue,the click will happen
        @param isOptimized              if use bit to store the alpha data, it will use smaller memory       
    */
    static irregularButton* create(const std::string& normalImage, const std::string& selectedImage = "", const std::string& disableImage = "",int alphaCheckValue = 0, bool isOptimized = false, Widget::TextureResType texType = Widget::TextureResType::LOCAL);
    /*
      @param alphaCheckValue          will use "btn" normalImage,selectedImage and disableImage to init the  irregularButton,will not change the "btn"
      @param alphaCheckValue          if image's alphaValue larger than alphaCheckValue,the click will happen
      @param isOptimized              if use bit to store the alpha data, it will use smaller memory
  */
    static irregularButton* createWithButton(Button * btn, int alphaCheckValue = 0, bool isOptimized = false);
protected:
    bool getIsTransparentAtPoint(cocos2d::Vec2 point) const;//获取点击到的像素数据是否大于checkAlphaValue
    void loadNormalTransparentInfo(std::string normalImage);   //初始化按钮
private:

    int _alphaCheckValue;
    int normalImageWidth_;
    int normalImageHeight_;

    bool _isOptimized;
    int _alphaDataWidth;
    int _alphaDataHeight;
    unsigned char * _imageAlpha;
    unsigned int _alphaDataLength;
};


#endif