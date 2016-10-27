/* Geometry shader to detect ray-triangle intersections */

#version 400 core

struct Ray {
	vec3 origin;
	vec3 direction;
};

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


uniform Ray ray;

out vec2 uv;
out float t;

bool calcIntersection();

void main() {
	if (calcIntersection() == false) {
		uv = vec2(0, 0);
		t = 0;
		EndPrimitive();
	};

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
}

bool calcIntersection() {
	const float epsilon = 0.0000001;

	/* get two triangle edges: */
	vec3 edge1 = (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
	vec3 edge2 = (gl_in[2].gl_Position- gl_in[0].gl_Position).xyz;

	/* normalize the rays direction vector: */
	vec3 dir = normalize(ray.direction);

	/* get the tripple product of edge1, dir and edge2: */
	vec3 pvec = cross(dir, edge2);
	float det = dot(edge1, pvec);

	/* if det is nearly 0, the ray and the triangle are lying in the same plane
	   and we can abort: */
	if (det < epsilon) return false;

	/* get the vector from vertex0 to rays origin: */
	vec3 vertexToRay = ray.origin - gl_in[0].gl_Position.xyz;

	/* we can now calculate the barycentric u-coordinate: */
	float uvx = dot(vertexToRay, pvec);
	if (uvx < 0.0f || uvx > det) return false;

	/* similar procedure for the barycentric v coordinate: */
	vec3 qvec = cross(vertexToRay, edge1);
	float uvy = dot(dir, qvec);

	if (uvy < 0.0f || uvx + uvy > det) return false;

	/* we have an intersection. prepare output: */
	uv.x = uvx;
	uv.y = uvy;
	t = dot(edge2, qvec);

	float invDet = 1.0f / det;

	uv.x *= invDet;
	uv.y *= invDet;
	t *= invDet;

	return true;
}