#pragma once
#include <Base/Base.h>
#include <Base/Manager/Dx11Manager.h>

/// @brief 名前コンポーネント
struct Tag : ComponentBase<Tag>
{
	/*template<typename ...Args>
	inline Tag(
	    Args&&... _args) :
	    content{ std::forward<Args>(_args)... }
	{

	}

	/// @brief 名前コンポーネントの初期化実装
	/// @param _tags 初期設定するタグ
	inline Tag(
	    const std::initializer_list<std::string>& _tags) :
	    content{ _tags }
	{
	    for (auto& tag : _tags)
	    {
	        content.emplace(tag);
	    }
	}*/

	/// @brief 名前コンポーネントの初期化処理
	constexpr Tag() = default;

	/// @brief 指定のタグが含まれているか
	/// @param _tag タグ名
	/// @return 含まれている true / false
	inline bool Has(
	    const std::string& _tag) const
	{
		return content.count(_tag) > 0;
	}

	/// @brief タグを追加する
	/// @param _tag タグ名
	inline void Add(
	    const std::string& _tag)
	{
		content.insert(_tag);
	}
	/// @brief タグを削除する
	/// @param _tag タグ名
	inline void Remove(
	    const std::string& _tag)
	{
		content.erase(_tag);
	}

	std::set<std::string> content{};  ///< タグの内容
};
