#include <Game/Game.h>

/// @brief メンバーリストのデータ
struct MemberListData
{
	inline static const size_t MEMBER_LIST_SIZE{ 9 };  ///< メンバーリストに入るメンバーの人数

	const Vec2 ELEMENT_SCALE_PIX{ 640, 80 };   ///< リストの名前要素のサイズ(ピクセル)
	Resource<Texture> texture{ "Image/Lobby/JoinnerList.png" };  ///< メンバーリストの背景テクスチャ

	Component<UITransform> transform{ Vec2{ 600, 40 }, Vec2{ 722, 722 } };  ///< UI座標系
	Component<UIImage> image{};                                             ///< UI画像

	std::array<std::string, MEMBER_LIST_SIZE> memberNames{};  ///< メンバー一覧のリスト

	std::vector<GameObject> memberObjElements{};  ///< メンバーリストの要素ゲームオブジェクト
};

/// @brief メンバーリスト
struct MemberList : ObjectBase<MemberListData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief メンバーリストを更新する
	void UpdateMemberList();

	/// @brief メンバーをリストから消す
	/// @param _index リスト上のインデックス
	void RemoveMember(const size_t _index);
	/// @brief メンバーをセットする
	/// @param _index リスト上のインデックス
	/// @param _name メンバーの名前
	void SetMember(const size_t _index, const std::string_view _name);
};
