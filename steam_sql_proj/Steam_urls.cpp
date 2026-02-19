#include "Steam_urls.h"
#include <format>

namespace badSQL
{
	std::string makeurl_player_summary(std::string_view api_key, std::string_view steamid)noexcept
	{
		return std::format(
			"{}?key={}&steamids={}&format=json",
			my_urls::player_summary_urlbase,
			api_key,
			steamid
		);
	}
	std::string makeurl_player_level(std::string_view api_key, std::string_view steamid)noexcept
	{
		return std::format(
			"{}?key={}&steamid=",
			my_urls::player_level_urlbase,
			api_key,
			steamid
		);
	}
	std::string makeurl_player_owned_games(std::string_view api_key, std::string_view steamid)noexcept
	{
		return std::format(
			"{}?key={}&steamid={}&include_appinfo=true&include_played_free_games=true&format=json",
			my_urls::player_owned_games_urlbase,
			api_key,
			steamid
		);
	}
	std::string makeurl_player_achievements(std::string_view api_key, std::string_view steamid, std::string_view appid)noexcept
	{
		return std::format(
			"{}?key={}&steamid={}&appid={}&l=en",
			my_urls::payer_achv_baseurl,
			api_key,
			steamid,
			appid
		);
	}
	std::string makeurl_global_achv_percentages(std::string_view appid)noexcept
	{
		return std::format("{}?gameid={}&format=json",
			my_urls::global_achv_percentages_baseurl,
			appid);
	}
	std::string makeurl_players_activity(std::string_view api_key, std::string_view steamid)noexcept
	{
		return std::format(
			"{}?key={}&steamid={}&format=json",
			my_urls::players_recent_activity_baseurl,
			api_key,
			steamid
		);
	}
}