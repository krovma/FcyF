#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Mat4.hpp"

//#Todo: All vertices should in CW

////////////////////////////////
void AddVerticesOfDisk2D(std::vector<Vertex_PCU>& verts, const Vec2& center, float radius, const Rgba& color, int sides /*= 64 */)
{
	Vec3* diskVerts = new Vec3[sides + 1];
	for (int i = 0; i < sides; ++i) {
		diskVerts[i] = Vec3(center + Vec2::MakeFromPolarDegrees(((float)i / 32.f * 360.f), radius));
	}
	diskVerts[sides] = diskVerts[0];
	
	{
		Vec3 center3(center);
		for (int i = 0; i < sides; ++i) {
			const Vec3 &crt = diskVerts[i];
			const Vec3 &nxt = diskVerts[i + 1];
			verts.push_back(Vertex_PCU(crt, color, Vec2()));
			verts.push_back(Vertex_PCU(nxt, color, Vec2()));
			verts.push_back(Vertex_PCU(center3, color, Vec2()));
		}
	}
	delete[] diskVerts;
}

////////////////////////////////
void AddVerticesOfLine2D(std::vector<Vertex_PCU>& verts, const Vec2& start, const Vec2& end, float thickness, const Rgba& color)
{
	Vec2 thicknessModifier = end - start;
	thicknessModifier.SetLength(thickness / 2.f);

	Vec3 endLeft(end + thicknessModifier + thicknessModifier.GetRotated90Degrees());
	Vec3 endRight(end + thicknessModifier + thicknessModifier.GetRotatedMinus90Degrees());
	Vec3 startLeft(start - thicknessModifier + thicknessModifier.GetRotated90Degrees());
	Vec3 startRight(start - thicknessModifier + thicknessModifier.GetRotatedMinus90Degrees());

	verts.push_back(Vertex_PCU(startLeft, color, Vec2()));
	verts.push_back(Vertex_PCU(endRight, color, Vec2()));
	verts.push_back(Vertex_PCU(endLeft, color, Vec2()));

	verts.push_back(Vertex_PCU(startLeft, color, Vec2()));
	verts.push_back(Vertex_PCU(startRight, color, Vec2()));
	verts.push_back(Vertex_PCU(endRight, color, Vec2()));
}

////////////////////////////////
void AddVerticesOfRing2D(std::vector<Vertex_PCU>& verts, const Vec2& center, float radius, float thickness, const Rgba& color, int sides /*= 64 */, float z/*= 0.f*/)
{
	Vec3* diskVerts = new Vec3[sides + 1];
	for (int i = 0; i < sides; ++i) {
		diskVerts[i] = Vec3(center + Vec2::MakeFromPolarDegrees(((float)i / 32.f * 360.f), radius), z);
	}
	diskVerts[sides] = diskVerts[0];

	{
		Vec3 center3(center);
		for (int i = 0; i < sides; ++i) {
			const Vec3 &crt = diskVerts[i];
			const Vec3 &nxt = diskVerts[i + 1];
			Vec3 sModifier = crt - center3;
			sModifier.SetLength(thickness / 2.f);
			Vec3 sL = crt - sModifier;
			Vec3 sR = crt + sModifier;
			Vec3 eModifier = nxt - center3;
			eModifier.SetLength(thickness / 2.f);
			Vec3 eL = nxt - eModifier;
			Vec3 eR = nxt + eModifier;

			verts.push_back(Vertex_PCU(sL, color, Vec2()));
			verts.push_back(Vertex_PCU(eR, color, Vec2()));
			verts.push_back(Vertex_PCU(eL, color, Vec2()));

			verts.push_back(Vertex_PCU(sL, color, Vec2()));
			verts.push_back(Vertex_PCU(sR, color, Vec2()));
			verts.push_back(Vertex_PCU(eR, color, Vec2()));
		}
	}
	delete[] diskVerts;
}

////////////////////////////////
void AddVerticesOfAABB2D(std::vector<Vertex_PCU>& verts, const AABB2& box, const Rgba& color /*= Rgba(1.0, 1.0, 1.0)*/, const Vec2& bottomLeftTexCoord /*= Vec2(0.f, 1.f)*/, const Vec2& topRightTexCoord /*= Vec2(1.f, 0.f) */)
{
	Vec3 bottomLeft(box.Min);
	Vec3 bottomRight(box.Max.x, box.Min.y, 0);
	Vec3 topLeft(box.Min.x, box.Max.y, 0);
	Vec3 topRight(box.Max);
	
	verts.push_back(Vertex_PCU(bottomLeft, color, bottomLeftTexCoord));
	verts.push_back(Vertex_PCU(topRight, color, topRightTexCoord));
	verts.push_back(Vertex_PCU(topLeft, color, Vec2(bottomLeftTexCoord.x, topRightTexCoord.y)));

	verts.push_back(Vertex_PCU(bottomLeft, color, bottomLeftTexCoord));
	verts.push_back(Vertex_PCU(bottomRight, color, Vec2(topRightTexCoord.x, bottomLeftTexCoord.y)));
	verts.push_back(Vertex_PCU(topRight, color, topRightTexCoord));
}

////////////////////////////////
Vec2 TransformedPosition(const Vec2& position, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	Vec2 scaled = position * uniformScaleXY;
	Vec2 rotated = scaled.GetRotatedDegreesAboutOrigin(rotationDegreesAboutZ);
	Vec2 translated = rotated + translationXY;
	return translated;
}

////////////////////////////////
Vec3 TransformedPosition(const Vec3& position, float uniformScaleXY, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	Vec2 trunced = Vec2(position.x, position.y);
	Vec2 scaled = trunced * uniformScaleXY;
	Vec2 rotated = scaled.GetRotatedDegreesAboutOrigin(rotationDegreesAboutZ);
	Vec2 translated = rotated + translationXY;
	return Vec3(translated, position.z);
}

////////////////////////////////
void TransformVertex(Vertex_PCU& vertex, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	Vec3 newPos = TransformedPosition(vertex.m_position, uniformScale, rotationDegreesAboutZ, translationXY);
	vertex.m_position = newPos;
}

////////////////////////////////
void TransformVertexArray(int numVertexes, Vertex_PCU vertices[], float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	Mat4 transform;
	transform *= Mat4::MakeTranslate2D(translationXY);
	transform *= Mat4::MakeRotateDegrees2D(rotationDegreesAboutZ);
	transform *= Mat4::MakeUniformScale2D(uniformScale);
	for (int i = 0; i < numVertexes; ++i) {
		vertices[i].m_position = transform.GetTransformedPosition3D(vertices[i].m_position);
	}
}

////////////////////////////////
void TransformVertexArray(size_t numVertexes, std::vector<Vertex_PCU>& vertices, float uniformScale, float rotationDegreesAboutZ, const Vec2& translationXY)
{
	TransformVertexArray((int)numVertexes, vertices.data(), uniformScale, rotationDegreesAboutZ, translationXY);
}
