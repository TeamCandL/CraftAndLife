#include "pch\pch.h"
#include "TestImageCircle.h"

void TestImageCircle::Update()
{

	ImGui::Begin("TestImageCircle");
	ImGui::DragFloat("min", &image.minRadius, 0.01f);
	ImGui::DragFloat("max", &image.maxRadius, 0.01f);
	ImGui::DragFloat("begin", &image.beginAngleRad, 0.01f);
	ImGui::DragFloat("end", &image.sizeAngleRad, 0.01f);

	ImGui::End();
}
