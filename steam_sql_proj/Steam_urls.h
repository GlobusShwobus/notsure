#pragma once

#include <string>
#include <string_view>

namespace badSQL
{
	namespace my_urls
	{
		static constexpr std::string_view player_summary_urlbase =
			"https://api.steampowered.com/ISteamUser/GetPlayerSummaries/v0002/";

		static constexpr std::string_view player_level_urlbase =
			"https://api.steampowered.com/IPlayerService/GetSteamLevel/v1/";

		static constexpr std::string_view player_owned_games_urlbase =
			"https://api.steampowered.com/IPlayerService/GetOwnedGames/v0001/";

		static constexpr std::string_view payer_achv_baseurl =
			"https://api.steampowered.com/ISteamUserStats/GetPlayerAchievements/v1/";

		static constexpr std::string_view global_achv_percentages_baseurl =
			"https://api.steampowered.com/ISteamUserStats/GetGlobalAchievementPercentagesForApp/v0002/";

		static constexpr std::string_view players_recent_activity_baseurl =
			"https://api.steampowered.com/IPlayerService/GetRecentlyPlayedGames/v0001/";
	}


	std::string makeurl_player_summary                  (std::string_view api_key, std::string_view steamid)noexcept;
	std::string makeurl_player_level                    (std::string_view api_key, std::string_view steamid)noexcept;
	std::string makeurl_player_owned_games              (std::string_view api_key, std::string_view steamid)noexcept;
	std::string makeurl_player_achievements             (std::string_view api_key, std::string_view steamid, std::string_view appid)noexcept;
	std::string makeurl_global_achv_percentages         (std::string_view appid)noexcept;
	std::string makeurl_players_activity                (std::string_view api_key, std::string_view steamid)noexcept;

	//may or may not need these
	//static constexpr std::string_view urlT_userSteamBadges = "http://api.steampowered.com/IPlayerService/GetBadges/v1/?key=&steamid=";//badge info
}