#include "pch\pch.h"
#include "MyGauge.h"
#include "../Player.h"

void MyGauge::Start()
{
	// ゲージの初期幅を設定
	transform.scale.x = maxWidth;
}

void MyGauge::Update()
{
	// プレイヤーを取得
	Player& player = Get<Player>(objPlayer);

	// 最大HPが0以下なら計算しない
	if (player.Data().maxHP <= 0)
	{
		transform.scale.x = 0.0f;
		return;
	}

	// HPの割合を計算（0.0～1.0）
	float hpRate = static_cast<float>(player.Data().hp) / static_cast<float>(player.Data().maxHP);

	// 割合を0～1の範囲に制限
	if (hpRate < 0.0f)
		hpRate = 0.0f;

	if (hpRate > 1.0f)
		hpRate = 1.0f;

	// HPの割合に応じてゲージの幅を変更
	transform.scale.x = maxWidth * hpRate;
}
