##A gif lib for cocos2dx-3.x
===================

This project is based on the gif_lib, and some codes reference to the android skia.
In my project, gif_lib library version is 5, and there are some changes to adapt android, ios, win32 platform.
The cocos2dx version number is 2.2.0 


##Use 
`Gif widget` just like a `Sprite`:

	std::string name = "g1.gif";
	name = FileUtils::getInstance()->fullPathForFilename(name.c_str());
	GifBase *gif = InstantGif::create(name.c_str());//InstantGif ：While playing, while parsing
	gif->setAnchorPoint(Point(0,0));
	this->addChild(gif);
	
	gif = CacheGif::create(name);//CacheGif: onece parse, and cache.
	gif->setPosition(ccp(500,0));
	gif->setScale(2);
	this->addChild(gif);
	
Special attention should be paid that the gif file must be placed in sdcard rather than in the asset dir on android platform

## License

```
Copyright 2013 Chang Shuai

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
    
```

##Contact Me

 If you have any question, please contact with me.
 email: chshuai.mail@gmail.com
 qq:343179869

