#include "Collider.h"
#include <algorithm>

bool learning::Intersect(ColliderCircle const& lhs, ColliderCircle const& rhs)
{
	return (rhs.center - lhs.center).LengthSquared() <= pow(lhs.radius + rhs.radius, 2);
}

bool learning::Intersect(ColliderBox const& lhs, ColliderBox const& rhs)
{
	//x-direction
	if (lhs.center.x - lhs.halfSize.x > rhs.center.x + rhs.halfSize.x) return false;
	if (lhs.center.x + lhs.halfSize.x < rhs.center.x - rhs.halfSize.x) return false;

	//y-direction
	if (lhs.center.y - lhs.halfSize.y > rhs.center.y + rhs.halfSize.y) return false;
	if (lhs.center.y + lhs.halfSize.y < rhs.center.y - rhs.halfSize.y) return false;

	return true;

}

bool learning::Intersect(ColliderLine const& line, ColliderCircle const& circle)
{
    Vector2f A = line.startPoint;
    Vector2f B = line.endPoint;
    Vector2f C = circle.center;

    // 벡터 AB와 AC 도출
    Vector2f AB = { B.x - A.x, B.y - A.y };
    Vector2f AC = { C.x - A.x, C.y - A.y };

    // 내적 연산
    float dotAB_AB = (AB.x * AB.x) + (AB.y * AB.y);
    float dotAC_AB = (AC.x * AB.x) + (AC.y * AB.y);

    // 투영 비율 t 계산
    float t = 0.0f;

    // 선분의 길이가 0인 경우(시작점과 끝점이 같음) 0으로 처리 방지
    // t가 0보다 작으면 C와 가장 가까운 점은 A, t가 1보다 크면 C와 가장 가까운 점은 B
    if (dotAB_AB != 0.0f)
    {
        t = dotAC_AB / dotAB_AB;
    }

    // 비율 t를 0.0f ~ 1.0f 사이로 클램핑
    t = std::max(0.0f, std::min(1.0f, t));

    // 선분 위에서 원 중심과 가장 가까운 지점 P 좌표 도출
    Vector2f closestPoint = { A.x + (AB.x * t), A.y + (AB.y * t) };

    // 점 P와 원 중심 C 사이의 거리 벡터
    Vector2f distVector = { C.x - closestPoint.x, C.y - closestPoint.y };

    // 거리의 제곱 도출
    float distanceSquared = (distVector.x * distVector.x) + (distVector.y * distVector.y);
    float radiusSquared = static_cast<float>(circle.radius * circle.radius);

    // 거리 제곱과 반지름 제곱 비교
    return distanceSquared <= radiusSquared;
}