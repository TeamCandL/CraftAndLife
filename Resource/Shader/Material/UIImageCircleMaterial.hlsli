// 円形に切り取るデータ
cbuffer UIImageCircleMaterial : register(b6)
{
    // x: 円中心からの切り抜き半径
    // y: 円中心からの表示半径
    // z: 円の開始角度(ラジアン)
    // w: 円の終了角度(ラジアン)
    float4 UIImageCircleMaterial_MinRadius_MaxRadius_BeginAngleRad_SizeAngleRad;
}
