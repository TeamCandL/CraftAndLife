#include "Reticle.h"

void Reticle::Start()
{
	image.texture = texture;

	const Vec2 SCREEN_SIZE = Get<ScreenSize>();
	auto& textureaa = image.texture.Get();
	transform.scale = { 140, 140 };
	transform.position = (SCREEN_SIZE - transform.scale) * 0.5f;
}
