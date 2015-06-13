
#include <Base.h>

/**
@brief	エフェクトを表示する。
*/
void MapObject2D()
{
	// aceを初期化する
	asd::Engine::Initialize(asd::ToAString("MapObject2D").c_str(), 640, 480, asd::EngineOption());

	// シーンを生成する
	auto scene = std::make_shared<asd::Scene>();

	// レイヤーを生成する
	auto layer = std::make_shared<asd::Layer2D>();

	// シーンにレイヤーを追加する
	scene->AddLayer(layer);

	// シーンを切り替える
	asd::Engine::ChangeScene(scene);

	//カメラを設定する。
	auto camera = std::make_shared<asd::CameraObject2D>();

	camera->SetSrc(asd::RectI(0, 0, 640, 480));
	camera->SetDst(asd::RectI(0, 0, 640, 480));

	{
		//マップオブジェクトを生成する。
		auto mapObject = std::make_shared<asd::MapObject2D>();

		auto texture = asd::Engine::GetGraphics()->CreateTexture2D(asd::ToAString("Data/Texture/Sample2.png").c_str());

		//マップオブジェクトに50*50=2500個のチップを登録する。
		for (int i = 0; i < 50; ++i)
		{
			for (int j = 0; j < 50; ++j)
			{
				//チップを生成する。
				auto chip = std::make_shared<asd::Chip2D>();

				//チップにテクスチャを設定する。
				chip->SetTexture(texture);

				//チップの描画先を指定する。
				chip->SetPosition(asd::Vector2DF(i * 40 - 1000, j * 40 - 1000));

				//マップオブジェクトにチップを追加する。
				mapObject->AddChip(chip);
			}
		}

		//レイヤーにマップオブジェクトを追加する。
		layer->AddObject(mapObject);

		//レイヤーにカメラオブジェクトを追加する。
		layer->AddObject(camera);
	}

	// aceが進行可能かチェックする。
	while (asd::Engine::DoEvents())
	{
		//カメラを移動させる
		auto pos = camera->GetSrc();
		pos.X += 1;
		pos.Y += 1;
		pos.X %= 1000;
		pos.Y %= 1000;
		camera->SetSrc(pos);

		// aceを更新する。
		asd::Engine::Update();
	}

	// aceを終了する。
	asd::Engine::Terminate();
}