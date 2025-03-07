/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.				*/
#ifndef ENGINE_SHARED_CONFIG_VARIABLES_H
#define ENGINE_SHARED_CONFIG_VARIABLES_H
#undef ENGINE_SHARED_CONFIG_VARIABLES_H // this file will be included several times

// TODO: remove this
#include "././game/variables.h"

MACRO_CONFIG_STR(Password, password, 32, "", CFGFLAG_CLIENT | CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, "Password to the server")
MACRO_CONFIG_STR(Logfile, logfile, 128, "", CFGFLAG_SAVE | CFGFLAG_CLIENT | CFGFLAG_SERVER, "Filename to log all output to")
MACRO_CONFIG_INT(Loglevel, loglevel, 0, -3, 2, CFGFLAG_SAVE | CFGFLAG_CLIENT | CFGFLAG_SERVER, "Adjusts the amount of information in the logfile (-3 = none, -2 = error only, -1 = warn, 0 = info, 1 = debug, 2 = trace)")
MACRO_CONFIG_INT(StdoutOutputLevel, stdout_output_level, 0, -3, 2, CFGFLAG_SAVE | CFGFLAG_CLIENT | CFGFLAG_SERVER, "Adjusts the amount of information in the system console (-3 = none, -2 = error only, -1 = warn, 0 = info, 1 = debug, 2 = trace)")
MACRO_CONFIG_INT(ConsoleOutputLevel, console_output_level, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SERVER, "Adjusts the amount of information in the console")
MACRO_CONFIG_INT(ConsoleEnableColors, console_enable_colors, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SERVER, "Enable colors in console output")

MACRO_CONFIG_STR(SvName, sv_name, 128, "unnamed server", CFGFLAG_SERVER, "Server name")
MACRO_CONFIG_STR(Bindaddr, bindaddr, 128, "", CFGFLAG_SERVER|CFGFLAG_MASTER, "Address to bind the client/server to")
MACRO_CONFIG_INT(SvIpv4Only, sv_ipv4only, 0, 0, 1, CFGFLAG_SERVER, "Whether to bind only to ipv4, otherwise bind to all available interfaces")
MACRO_CONFIG_INT(SvPort, sv_port, 8303, 0, 0, CFGFLAG_SERVER, "Port to use for the server")
MACRO_CONFIG_STR(SvHostname, sv_hostname, 128, "", CFGFLAG_SAVE | CFGFLAG_SERVER, "Server hostname (0.7 only)")
MACRO_CONFIG_STR(SvMap, sv_map, 128, "", CFGFLAG_SERVER, "Map to use on the server")
MACRO_CONFIG_INT(SvMaxClients, sv_max_clients, 64, 1, MAX_CLIENTS, CFGFLAG_SERVER, "Maximum number of clients that are allowed on a server")
MACRO_CONFIG_INT(SvMaxClientsPerIP, sv_max_clients_per_ip, 4, 1, MAX_CLIENTS, CFGFLAG_SERVER, "Maximum number of clients with the same IP that can connect to the server")
MACRO_CONFIG_INT(SvHighBandwidth, sv_high_bandwidth, 0, 0, 1, CFGFLAG_SERVER, "Use high bandwidth mode. Doubles the bandwidth required for the server. LAN use only")
MACRO_CONFIG_STR(SvRegister, sv_register, 16, "1", CFGFLAG_SERVER, "Register server with master server for public listing, can also accept a comma-separated list of protocols to register on, like 'ipv4,ipv6'")
MACRO_CONFIG_STR(SvRegisterExtra, sv_register_extra, 256, "", CFGFLAG_SERVER, "Extra headers to send to the register endpoint, comma separated 'Header: Value' pairs")
MACRO_CONFIG_STR(SvRegisterUrl, sv_register_url, 128, "https://master1.ddnet.org/ddnet/15/register", CFGFLAG_SERVER, "Masterserver URL to register to")
MACRO_CONFIG_STR(SvRconPassword, sv_rcon_password, 32, "", CFGFLAG_SERVER, "Remote console password (full access)")
MACRO_CONFIG_STR(SvRconModPassword, sv_rcon_mod_password, 32, "", CFGFLAG_SERVER, "Remote console password for moderators (limited access)")
MACRO_CONFIG_STR(SvRconHelperPassword, sv_rcon_helper_password, 128, "", CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, "Remote console password for helpers (limited access)")
MACRO_CONFIG_INT(SvRconMaxTries, sv_rcon_max_tries, 3, 0, 100, CFGFLAG_SERVER, "Maximum number of tries for remote console authentication")
MACRO_CONFIG_INT(SvRconBantime, sv_rcon_bantime, 5, 0, 1440, CFGFLAG_SERVER, "The time a client gets banned if remote console authentication fails. 0 makes it just use kick")
MACRO_CONFIG_INT(SvRconTokenCheck, sv_rcon_token_check, 1, 0, 1, CFGFLAG_SERVER, "Require the use of a client with tokenized protection against IP address spoofing to permit access to the console")
MACRO_CONFIG_INT(SvAutoDemoRecord, sv_auto_demo_record, 0, 0, 1, CFGFLAG_SERVER, "Automatically record demos")
MACRO_CONFIG_INT(SvAutoDemoMax, sv_auto_demo_max, 10, 0, 1000, CFGFLAG_SERVER, "Maximum number of automatically recorded demos (0 = no limit)")
MACRO_CONFIG_INT(SvVanillaAntiSpoof, sv_vanilla_antispoof, 0, 0, 1, CFGFLAG_SERVER, "Enable vanilla Antispoof")
MACRO_CONFIG_INT(SvRconVote, sv_rcon_vote, 0, 0, 1, CFGFLAG_SERVER, "Only allow authed clients to call votes")

MACRO_CONFIG_INT(SvPlayerDemoRecord, sv_player_demo_record, 0, 0, 1, CFGFLAG_SERVER, "Automatically record demos for each player")
MACRO_CONFIG_INT(SvDemoChat, sv_demo_chat, 0, 0, 1, CFGFLAG_SERVER, "Record chat for demos")
MACRO_CONFIG_INT(SvServerInfoPerSecond, sv_server_info_per_second, 10, 1, 1000, CFGFLAG_SERVER, "Maximum number of complete server info responses that are sent out per second")
MACRO_CONFIG_INT(SvVanConnPerSecond, sv_van_conn_per_second, 10, 0, 10000, CFGFLAG_SERVER, "Antispoof specific ratelimit (0 for no limit)")
MACRO_CONFIG_INT(SvSixup, sv_sixup, 0, 0, 1, CFGFLAG_SERVER, "Enable sixup connections")
MACRO_CONFIG_INT(SvSkillLevel, sv_skill_level, 1, SERVERINFO_LEVEL_MIN, SERVERINFO_LEVEL_MAX, CFGFLAG_SERVER, "Difficulty level for Teeworlds 0.7 (0: Casual, 1: Normal, 2: Competitive)")

MACRO_CONFIG_STR(EcBindaddr, ec_bindaddr, 128, "localhost", CFGFLAG_ECON, "Address to bind the external console to. Anything but 'localhost' is dangerous")
MACRO_CONFIG_INT(EcPort, ec_port, 0, 0, 0, CFGFLAG_ECON, "Port to use for the external console")
MACRO_CONFIG_STR(EcPassword, ec_password, 32, "", CFGFLAG_ECON, "External console password")
MACRO_CONFIG_INT(EcBantime, ec_bantime, 0, 0, 1440, CFGFLAG_ECON, "The time a client gets banned if econ authentication fails. 0 just closes the connection")
MACRO_CONFIG_INT(EcAuthTimeout, ec_auth_timeout, 30, 1, 120, CFGFLAG_ECON, "Time in seconds before the the econ authentification times out")
MACRO_CONFIG_INT(EcOutputLevel, ec_output_level, 1, 0, 2, CFGFLAG_ECON, "Adjusts the amount of information in the external console")

MACRO_CONFIG_INT(Debug, debug, 0, 0, 1, CFGFLAG_SERVER, "Debug mode")
MACRO_CONFIG_INT(DbgCurl, dbg_curl, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SERVER, "Debug curl")
MACRO_CONFIG_INT(DbgPref, dbg_pref, 0, 0, 1, CFGFLAG_SERVER, "Performance outputs")
MACRO_CONFIG_INT(DbgHitch, dbg_hitch, 0, 0, 0, CFGFLAG_SERVER, "Hitch warnings")
#ifdef CONF_DEBUG
MACRO_CONFIG_INT(DbgStress, dbg_stress, 0, 0, 0, CFGFLAG_SERVER, "Stress systems")
MACRO_CONFIG_INT(DbgStressNetwork, dbg_stress_network, 0, 0, 0, CFGFLAG_SERVER, "Stress network")
#endif

MACRO_CONFIG_INT(HttpAllowInsecure, http_allow_insecure, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SERVER, "Allow insecure HTTP protocol in addition to the secure HTTPS one. Mostly useful for testing.")

MACRO_CONFIG_INT(SvReservedSlots, sv_reserved_slots, 0, 0, MAX_CLIENTS, CFGFLAG_SERVER, "The number of slots that are reserved for special players")
MACRO_CONFIG_STR(SvReservedSlotsPass, sv_reserved_slots_pass, 128, "", CFGFLAG_SERVER | CFGFLAG_NONTEEHISTORIC, "The password that is required to use a reserved slot")
MACRO_CONFIG_INT(SvReservedSlotsAuthLevel, sv_reserved_slots_auth_level, 1, 1, 4, CFGFLAG_SERVER, "Minimum rcon auth level needed to use a reserved slot. 4 = rcon auth disabled")
MACRO_CONFIG_STR(SvBroadcast, sv_broadcast, 64, "DDRace.info Trunk 0.5", CFGFLAG_SERVER, "The broadcasting message")
MACRO_CONFIG_INT(SvShutdownWhenEmpty, sv_shutdown_when_empty, 0, 0, 1, CFGFLAG_SERVER, "Shutdown server as soon as noone is on it anymore")
MACRO_CONFIG_INT(SvReloadWhenEmpty, sv_reload_when_empty, 0, 0, 2, CFGFLAG_SERVER, "Reload map when server is empty (1 = reload once, 2 = reload every time server gets empty)")

MACRO_CONFIG_INT(SvEmoticonDelay, sv_emoticon_delay, 0, 0, 9999, CFGFLAG_SERVER, "The time in seconds between over-head emoticons")

// netlimit
MACRO_CONFIG_INT(SvNetlimit, sv_netlimit, 0, 0, 10000, CFGFLAG_SERVER, "Netlimit: Maximum amount of traffic a client is allowed to use (in kb/s)")
MACRO_CONFIG_INT(SvNetlimitAlpha, sv_netlimit_alpha, 50, 1, 100, CFGFLAG_SERVER, "Netlimit: Alpha of Exponention moving average")

MACRO_CONFIG_INT(SvConnlimit, sv_connlimit, 5, 0, 100, CFGFLAG_SERVER, "Connlimit: Number of connections an IP is allowed to do in a timespan")
MACRO_CONFIG_INT(SvConnlimitTime, sv_connlimit_time, 20, 0, 1000, CFGFLAG_SERVER, "Connlimit: Time in which IP's connections are counted")
MACRO_CONFIG_INT(SvDistConnlimit, sv_distconnlimit, 16, 0, 100, CFGFLAG_SERVER, "DistConnlimit: Number of connections (from all IPs) that is allowed to do in a timespan")
MACRO_CONFIG_INT(SvDistConnlimitTime, sv_distconnlimit_time, 60, 0, 1000, CFGFLAG_SERVER, "DistConnlimit: Time in which (all IP's) connections are counted")

MACRO_CONFIG_INT(SvPauseMessages, sv_pause_messages, 0, 0, 1, CFGFLAG_SERVER, "Whether to show messages when a player pauses and resumes")

MACRO_CONFIG_INT(SvChatDelay, sv_chat_delay, 1, 0, 9999, CFGFLAG_SERVER, "The time in seconds between chat messages")
MACRO_CONFIG_INT(SvInfoChangeDelay, sv_info_change_delay, 5, 0, 9999, CFGFLAG_SERVER, "The time in seconds between info changes (name/skin/color), to avoid ranbow mod set this to a very high time")
MACRO_CONFIG_INT(SvVoteTime, sv_vote_time, 25, 1, 9999, CFGFLAG_SERVER, "The time in seconds a vote lasts")
MACRO_CONFIG_INT(SvVoteMapTimeDelay, sv_vote_map_delay, 0, 0, 9999, CFGFLAG_SERVER, "The minimum time in seconds between map votes")
MACRO_CONFIG_INT(SvVoteDelay, sv_vote_delay, 3, 0, 9999, CFGFLAG_SERVER, "The time in seconds between any vote")

MACRO_CONFIG_INT(SvMapWindow, sv_map_window, 15, 0, 100, CFGFLAG_SERVER, "Map downloading send-ahead window")
MACRO_CONFIG_INT(SvFastDownload, sv_fast_download, 1, 0, 1, CFGFLAG_SERVER, "Enables fast download of maps")

MACRO_CONFIG_STR(SvRegionName, sv_region_name, 5, "UNK", CFGFLAG_SERVER, "Server region. Used for regional bans")
MACRO_CONFIG_INT(SvUseSQL, sv_use_sql, 0, 0, 1, CFGFLAG_SERVER, "Enables MySQL backend instead of SQLite backend (sv_sqlite_file is still used as fallback write server when no MySQL server is reachable)")
MACRO_CONFIG_INT(SvSqlQueriesDelay, sv_sql_queries_delay, 1, 0, 20, CFGFLAG_SERVER, "Delay in seconds between SQL queries of a single player")
MACRO_CONFIG_STR(SvSqliteFile, sv_sqlite_file, 64, "infclass-server.sqlite", CFGFLAG_SERVER, "File to store ranks in case sv_use_sql is turned off or used as backup sql server")
MACRO_CONFIG_STR(SvSqlBindaddr, sv_sql_bindaddr, 128, "", CFGFLAG_SERVER, "Address to bind the SQL connections to")

MACRO_CONFIG_INT(SvDDRaceRules, sv_ddrace_rules, 1, 0, 1, CFGFLAG_SERVER, "Whether the default mod rules are displayed or not")
MACRO_CONFIG_STR(SvRulesLine1, sv_rules_line1, 128, "", CFGFLAG_SERVER, "Rules line 1")
MACRO_CONFIG_STR(SvRulesLine2, sv_rules_line2, 128, "", CFGFLAG_SERVER, "Rules line 2")
MACRO_CONFIG_STR(SvRulesLine3, sv_rules_line3, 128, "", CFGFLAG_SERVER, "Rules line 3")
MACRO_CONFIG_STR(SvRulesLine4, sv_rules_line4, 128, "", CFGFLAG_SERVER, "Rules line 4")
MACRO_CONFIG_STR(SvRulesLine5, sv_rules_line5, 128, "", CFGFLAG_SERVER, "Rules line 5")
MACRO_CONFIG_STR(SvRulesLine6, sv_rules_line6, 128, "", CFGFLAG_SERVER, "Rules line 6")
MACRO_CONFIG_STR(SvRulesLine7, sv_rules_line7, 128, "", CFGFLAG_SERVER, "Rules line 7")
MACRO_CONFIG_STR(SvRulesLine8, sv_rules_line8, 128, "", CFGFLAG_SERVER, "Rules line 8")
MACRO_CONFIG_STR(SvRulesLine9, sv_rules_line9, 128, "", CFGFLAG_SERVER, "Rules line 9")
MACRO_CONFIG_STR(SvRulesLine10, sv_rules_line10, 128, "", CFGFLAG_SERVER, "Rules line 10")

MACRO_CONFIG_INT(SvTeam, sv_team, 1, 0, 3, CFGFLAG_SERVER | CFGFLAG_GAME, "Teams configuration (0 = off, 1 = on but optional, 2 = must play only with teams, 3 = forced random team only for you)")

MACRO_CONFIG_STR(SvAnnouncementFileName, sv_announcement_filename, 24, "announcement.txt", CFGFLAG_SERVER, "file which will have the announcement, each one at a line")
MACRO_CONFIG_INT(SvAnnouncementInterval, sv_announcement_interval, 300, 1, 9999, CFGFLAG_SERVER, "time(minutes) in which the announcement will be displayed from the announcement file")
MACRO_CONFIG_INT(SvAnnouncementRandom, sv_announcement_random, 1, 0, 1, CFGFLAG_SERVER, "Whether announcements are sequential or random")

MACRO_CONFIG_INT(SvShowOthersDefault, sv_show_others_default, 0, 0, 2, CFGFLAG_SERVER | CFGFLAG_GAME, "Whether players see others by default (2 for own team)")
MACRO_CONFIG_INT(SvShowAllDefault, sv_show_all_default, 0, 0, 1, CFGFLAG_SERVER, "Whether players see all tees by default")
MACRO_CONFIG_STR(SvShutdownFile, sv_shutdown_file, 128, "", CFGFLAG_SERVER, "File to execute on the server shut down")
MACRO_CONFIG_STR(SvResetFile, sv_reset_file, 128, "reset.cfg", CFGFLAG_SERVER, "File to execute on map change or reload to set the default server settings")

MACRO_CONFIG_INT(SvSlashMe, sv_slash_me, 0, 0, 1, CFGFLAG_SERVER, "Whether /me is active on the server or not")

MACRO_CONFIG_INT(ConnTimeout, conn_timeout, 100, 5, 1000, CFGFLAG_SAVE | CFGFLAG_CLIENT | CFGFLAG_SERVER, "Network timeout")
MACRO_CONFIG_INT(ConnTimeoutProtection, conn_timeout_protection, 1000, 5, 10000, CFGFLAG_SERVER, "Network timeout protection")

//Add cheat versions
MACRO_CONFIG_STR(SvBannedVersions, sv_banned_versions, 128, "", CFGFLAG_SERVER, "Comma separated list of banned clients to be kicked on join")

// demo editor
MACRO_CONFIG_INT(ClDemoSliceBegin, cl_demo_slice_begin, -1, 0, 0, CFGFLAG_SAVE | CFGFLAG_CLIENT, "Begin marker for demo slice")
MACRO_CONFIG_INT(ClDemoSliceEnd, cl_demo_slice_end, -1, 0, 0, CFGFLAG_SAVE | CFGFLAG_CLIENT, "End marker for demo slice")

// Infclass generic variables
MACRO_CONFIG_INT(SvHideInfo, sv_hide_info, 0, 0, 1, CFGFLAG_SERVER, "Hide the server info")
MACRO_CONFIG_INT(SvInfoMaxClients, sv_info_max_clients, -1, -1, 128, CFGFLAG_SERVER, "Limit the server info max clients number (-1 means 'unlimited')")
MACRO_CONFIG_INT(SvFilterChatCommands, sv_filter_chat_commands, 0, 0, 1, CFGFLAG_SERVER, "Omit from the chat messages starting with ! (like !me and !best)")
MACRO_CONFIG_INT(SvSuggestMoreRounds, sv_suggest_more_rounds, 0, 0, 100, CFGFLAG_SERVER, "The number of extra rounds to be played on the suggestion (vote) accepted")
MACRO_CONFIG_STR(SvChangeLogFile, sv_changelog_file, 128, "ChangeLog.txt", CFGFLAG_SERVER, "File with changelog entities")
MACRO_CONFIG_INT(SvChangeLogMaxLinesPerPage, sv_changelog_lines_page, 6, 1, 64, CFGFLAG_SERVER, "File with changelog entities")

#include "game/server/infclass/infc_config_variables.h"

#endif
