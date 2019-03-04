#ifndef QQTYPES_H
#define QQTYPES_H

namespace QuteQoin
{
	enum QQSmartUrlFilerTransformType { Full = 0, Short = 1, Shorter = 2};
	enum QQHuntMode { HuntMode_Disabled = 0, HuntMode_Manual = 1, HuntMode_Auto = 2};
	enum QQSLHuntMode { SLHuntMode_Disabled = 0, SLHuntMode_Silent = 1};

	struct QQBoardStates {
		bool hasBigorno : 1;
		bool hasError : 1;
		bool hasNewPosts : 1;
		bool hasResponse : 1;
		bool isHidden : 1;
	};
}

#endif // QQTYPES_H
