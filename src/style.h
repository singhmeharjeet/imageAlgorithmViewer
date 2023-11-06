//
// Created by Meharjeet Singh on 2023-11-03.
//

#ifndef IMAGE_STYLE_H
#define IMAGE_STYLE_H

#ifndef WIDTH
#define WIDTH 1200
#endif

#include <imgui.h>

#include <cstdio>


void AlignForWidth(float width, float alignment = 0.5f) {
	ImGuiStyle &style = ImGui::GetStyle();
	float avail = ImGui::GetContentRegionAvail().x;
	float off = (avail - width) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

void AlignForHeight(float height, float alignment = 0.5f) {
	ImGuiStyle &style = ImGui::GetStyle();
	float avail = ImGui::GetContentRegionAvail().y;
	float off = (avail - height) * alignment;
	if (off > 0.0f)
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + off);
}

void show_images(void *t1, void *t2, const char *label1, const char *label2, float image_width, float image_height) {
	// Setting the UI to center the images
	AlignForHeight(image_height, 0.25);
	AlignForWidth((2 * image_width));

	ImVec2 size = ImVec2(image_width, image_height);

	ImGui::Image(t1, size);
	ImGui::SameLine();
	ImGui::Image(t2, size);


	ImGui::SetCursorPosX(WIDTH * 0.2);
	ImGui::Text("%s", label1);
	ImGui::SameLine(WIDTH * 0.7);
	ImGui::Text("%s", label2);
}

#endif //IMAGE_STYLE_H
