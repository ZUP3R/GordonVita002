#include <math.h>
#include "vita2d.h"
#include "shared.h"

void reset_state()
{
	sceGxmSetFrontPointLineWidth(_vita2d_context, 1);
	sceGxmSetFrontPolygonMode(_vita2d_context, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
}

void vita2d_draw_pixel(float x, float y, unsigned int color)
{
	vita2d_color_vertex *vertex = (vita2d_color_vertex *)vita2d_pool_memalign(
		1 * sizeof(vita2d_color_vertex), // 1 vertex
		sizeof(vita2d_color_vertex));

	uint16_t *index = (uint16_t *)vita2d_pool_memalign(
		1 * sizeof(uint16_t), // 1 index
		sizeof(uint16_t));

	vertex->x = x;
	vertex->y = y;
	vertex->z = 0;
	vertex->color = color;

	*index = 0;

	sceGxmSetVertexProgram(_vita2d_context, _vita2d_colorVertexProgram);
	sceGxmSetFragmentProgram(_vita2d_context, _vita2d_colorFragmentProgram);

	void *vertexDefaultBuffer;
	sceGxmReserveVertexDefaultUniformBuffer(_vita2d_context, &vertexDefaultBuffer);
	sceGxmSetUniformDataF(vertexDefaultBuffer, _vita2d_colorWvpParam, 0, 16, _vita2d_ortho_matrix);

	sceGxmSetVertexStream(_vita2d_context, 0, vertex);
	sceGxmSetFrontPolygonMode(_vita2d_context, SCE_GXM_POLYGON_MODE_POINT);
	sceGxmDraw(_vita2d_context, SCE_GXM_PRIMITIVE_POINTS, SCE_GXM_INDEX_FORMAT_U16, index, 1);
	reset_state();
}

void vita2d_draw_line(float x0, float y0, float x1, float y1, unsigned int color)
{
	vita2d_draw_line_lw(x0, y0, x1, y1, color, 1);
}

void vita2d_draw_line_lw(float x0, float y0, float x1, float y1, unsigned int color, int linewidth)
{
	vita2d_color_vertex *vertices = (vita2d_color_vertex *)vita2d_pool_memalign(
		2 * sizeof(vita2d_color_vertex), // 2 vertices
		sizeof(vita2d_color_vertex));

	uint16_t *indices = (uint16_t *)vita2d_pool_memalign(
		2 * sizeof(uint16_t), // 2 indices
		sizeof(uint16_t));

	vertices[0].x = x0;
	vertices[0].y = y0;
	vertices[0].z = 0;
	vertices[0].color = color;

	vertices[1].x = x1;
	vertices[1].y = y1;
	vertices[1].z = 0;
	vertices[1].color = color;

	indices[0] = 0;
	indices[1] = 1;

	sceGxmSetVertexProgram(_vita2d_context, _vita2d_colorVertexProgram);
	sceGxmSetFragmentProgram(_vita2d_context, _vita2d_colorFragmentProgram);

	void *vertexDefaultBuffer;
	sceGxmReserveVertexDefaultUniformBuffer(_vita2d_context, &vertexDefaultBuffer);
	sceGxmSetUniformDataF(vertexDefaultBuffer, _vita2d_colorWvpParam, 0, 16, _vita2d_ortho_matrix);

	sceGxmSetVertexStream(_vita2d_context, 0, vertices);
	sceGxmSetFrontPointLineWidth(_vita2d_context, linewidth);
	sceGxmSetFrontPolygonMode(_vita2d_context, SCE_GXM_POLYGON_MODE_LINE);
	sceGxmDraw(_vita2d_context, SCE_GXM_PRIMITIVE_LINES, SCE_GXM_INDEX_FORMAT_U16, indices, 2);
	reset_state();
}

void vita2d_draw_rectangle(float x, float y, float w, float h, unsigned int color)
{
	vita2d_draw_rectangle_lw(x, y, w, h, color, 1);
}

void vita2d_draw_rectangle_lw(float x, float y, float w, float h, unsigned int color, int linewidth)
{
	int px = linewidth;
	vita2d_draw_fill_rectangle(x, y, w-px, px, color);
	vita2d_draw_fill_rectangle((x + w - px), y, px, h-px, color);
    vita2d_draw_fill_rectangle(x+px, (y + h - px), w-px, px, color);
	vita2d_draw_fill_rectangle(x, y+px, px, h-px, color);

	/*vita2d_color_vertex *vertices = (vita2d_color_vertex *)vita2d_pool_memalign(
		4 * sizeof(vita2d_color_vertex), // 4 vertices
		sizeof(vita2d_color_vertex));

	uint16_t *indices = (uint16_t *)vita2d_pool_memalign(
		8 * sizeof(uint16_t), // 8 indices
		sizeof(uint16_t));

	int lw = linewidth;
	if(lw == 1)
		lw = 0;

	vertices[0].x = x + lw/2;
	vertices[0].y = y + lw/2;
	vertices[0].z = 0;
	vertices[0].color = color;

	vertices[1].x = x + w - lw/2;
	vertices[1].y = y + lw/2;
	vertices[1].z = 0;
	vertices[1].color = color;

	vertices[2].x = x + w - lw/2;
	vertices[2].y = y + h - lw/2;
	vertices[2].z = 0;
	vertices[2].color = color;

	vertices[3].x = x + lw/2;
	vertices[3].y = y + h - lw/2;
	vertices[3].z = 0;
	vertices[3].color = color;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 1;
	indices[3] = 2;
	indices[4] = 2;
	indices[5] = 3;
	indices[6] = 3;
	indices[7] = 0;

	sceGxmSetVertexProgram(_vita2d_context, _vita2d_colorVertexProgram);
	sceGxmSetFragmentProgram(_vita2d_context, _vita2d_colorFragmentProgram);

	void *vertexDefaultBuffer;
	sceGxmReserveVertexDefaultUniformBuffer(_vita2d_context, &vertexDefaultBuffer);
	sceGxmSetUniformDataF(vertexDefaultBuffer, _vita2d_colorWvpParam, 0, 16, _vita2d_ortho_matrix);

	sceGxmSetVertexStream(_vita2d_context, 0, vertices);
	sceGxmSetFrontPointLineWidth(_vita2d_context, linewidth);
	sceGxmSetFrontPolygonMode(_vita2d_context, SCE_GXM_POLYGON_MODE_LINE);
	sceGxmDraw(_vita2d_context, SCE_GXM_PRIMITIVE_LINES, SCE_GXM_INDEX_FORMAT_U16, indices, 8);
	reset_state();
	*/
}

void vita2d_draw_fill_rectangle(float x, float y, float w, float h, unsigned int color)
{
	vita2d_color_vertex *vertices = (vita2d_color_vertex *)vita2d_pool_memalign(
		4 * sizeof(vita2d_color_vertex), // 4 vertices
		sizeof(vita2d_color_vertex));

	uint16_t *indices = (uint16_t *)vita2d_pool_memalign(
		4 * sizeof(uint16_t), // 4 indices
		sizeof(uint16_t));

	vertices[0].x = x;
	vertices[0].y = y;
	vertices[0].z = 0;
	vertices[0].color = color;

	vertices[1].x = x + w;
	vertices[1].y = y;
	vertices[1].z = 0;
	vertices[1].color = color;

	vertices[2].x = x + w;
	vertices[2].y = y + h;
	vertices[2].z = 0;
	vertices[2].color = color;

	vertices[3].x = x;
	vertices[3].y = y + h;
	vertices[3].z = 0;
	vertices[3].color = color;

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	sceGxmSetVertexProgram(_vita2d_context, _vita2d_colorVertexProgram);
	sceGxmSetFragmentProgram(_vita2d_context, _vita2d_colorFragmentProgram);

	void *vertexDefaultBuffer;
	sceGxmReserveVertexDefaultUniformBuffer(_vita2d_context, &vertexDefaultBuffer);
	sceGxmSetUniformDataF(vertexDefaultBuffer, _vita2d_colorWvpParam, 0, 16, _vita2d_ortho_matrix);

	sceGxmSetVertexStream(_vita2d_context, 0, vertices);
	sceGxmSetFrontPolygonMode(_vita2d_context, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
	sceGxmDraw(_vita2d_context, SCE_GXM_PRIMITIVE_TRIANGLE_FAN, SCE_GXM_INDEX_FORMAT_U16, indices, 4);
	reset_state();
}

void vita2d_draw_circle(float x, float y, float radius, unsigned int color)
{
	vita2d_draw_circle_lw(x, y, radius, color, 1);
}

void vita2d_draw_circle_lw(float x, float y, float radius, unsigned int color, int linewidth)
{
	static const int num_segments = 100;

	vita2d_color_vertex *vertices = (vita2d_color_vertex *)vita2d_pool_memalign(
		(num_segments + 1) * sizeof(vita2d_color_vertex),
		sizeof(vita2d_color_vertex));

	uint16_t *indices = (uint16_t *)vita2d_pool_memalign(
		(num_segments + 1) * sizeof(uint16_t),
		sizeof(uint16_t));

	float theta = 2 * M_PI / (float)num_segments;
	float c = cosf(theta);
	float s = sinf(theta);
	float t;

	float xx = radius;
	float yy = 0;
	int i;

	for (i = 0; i <= num_segments; i++) {
		vertices[i].x = x + xx;
		vertices[i].y = y + yy;
		vertices[i].z = 0;
		vertices[i].color = color;
		indices[i] = i;

		t = xx;
		xx = c * xx - s * yy;
		yy = s * t + c * yy;
	}

	indices[num_segments + 1] = 0;

	sceGxmSetVertexProgram(_vita2d_context, _vita2d_colorVertexProgram);
	sceGxmSetFragmentProgram(_vita2d_context, _vita2d_colorFragmentProgram);

	void *vertexDefaultBuffer;
	sceGxmReserveVertexDefaultUniformBuffer(_vita2d_context, &vertexDefaultBuffer);
	sceGxmSetUniformDataF(vertexDefaultBuffer, _vita2d_colorWvpParam, 0, 16, _vita2d_ortho_matrix);

	sceGxmSetVertexStream(_vita2d_context, 0, vertices);
	sceGxmSetFrontPointLineWidth(_vita2d_context, linewidth);
	sceGxmSetFrontPolygonMode(_vita2d_context, SCE_GXM_POLYGON_MODE_TRIANGLE_LINE);
	sceGxmDraw(_vita2d_context, SCE_GXM_PRIMITIVE_TRIANGLE_STRIP, SCE_GXM_INDEX_FORMAT_U16, indices, num_segments + 1);
	reset_state();
}

void vita2d_draw_fill_circle(float x, float y, float radius, unsigned int color)
{
	static const int num_segments = 100;

	vita2d_color_vertex *vertices = (vita2d_color_vertex *)vita2d_pool_memalign(
		(num_segments + 1) * sizeof(vita2d_color_vertex),
		sizeof(vita2d_color_vertex));

	uint16_t *indices = (uint16_t *)vita2d_pool_memalign(
		(num_segments + 2) * sizeof(uint16_t),
		sizeof(uint16_t));


	vertices[0].x = x;
	vertices[0].y = y;
	vertices[0].z = 0;
	vertices[0].color = color;
	indices[0] = 0;

	float theta = 2 * M_PI / (float)num_segments;
	float c = cosf(theta);
	float s = sinf(theta);
	float t;

	float xx = radius;
	float yy = 0;
	int i;

	for (i = 1; i <= num_segments; i++) {
		vertices[i].x = x + xx;
		vertices[i].y = y + yy;
		vertices[i].z = 0;
		vertices[i].color = color;
		indices[i] = i;

		t = xx;
		xx = c * xx - s * yy;
		yy = s * t + c * yy;
	}

	indices[num_segments + 1] = 1;

	sceGxmSetVertexProgram(_vita2d_context, _vita2d_colorVertexProgram);
	sceGxmSetFragmentProgram(_vita2d_context, _vita2d_colorFragmentProgram);

	void *vertexDefaultBuffer;
	sceGxmReserveVertexDefaultUniformBuffer(_vita2d_context, &vertexDefaultBuffer);
	sceGxmSetUniformDataF(vertexDefaultBuffer, _vita2d_colorWvpParam, 0, 16, _vita2d_ortho_matrix);

	sceGxmSetVertexStream(_vita2d_context, 0, vertices);
	sceGxmSetFrontPolygonMode(_vita2d_context, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);
	sceGxmDraw(_vita2d_context, SCE_GXM_PRIMITIVE_TRIANGLE_FAN, SCE_GXM_INDEX_FORMAT_U16, indices, num_segments + 2);
	reset_state();
}
