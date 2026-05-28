#pragma once
#include "Utillity.h"

namespace learning
{
	struct ColliderCircle
	{
		Vector2f center;
		double radius;
	};

	struct ColliderBox
	{
		Vector2f center;
		Vector2f halfSize;
	};

	// 마우스 궤적을 위한 선분 충돌체 정의
	struct ColliderLine
	{
		Vector2f startPoint;
		Vector2f endPoint;
	};


	// Circle
	bool Intersect(ColliderCircle const& lhs, ColliderCircle const& rhs);
	// AABB
	bool Intersect(ColliderBox const& lhs, ColliderBox const& rhs);
	// 선분과 원의 교차 판정 함수
	bool Intersect(ColliderLine const& line, ColliderCircle const& circle);
}

