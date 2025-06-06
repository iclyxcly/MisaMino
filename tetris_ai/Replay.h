#pragma once
#ifndef REPLAY_H
#define REPLAY_H
#include "json.hpp"
#include "tetrisgame.h"
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <cstdint>

namespace RP {
	using i8 = int8_t;
	using u8 = uint8_t;
	using i16 = int16_t;
	using u16 = uint16_t;
	using i32 = int32_t;
	using u32 = uint32_t;
	using i64 = int64_t;
	using u64 = uint64_t;

	using json = nlohmann::json;
	enum IGEType : u8 {
		I,
		IC
	};
	std::string igeKey[2] = {
		"interaction",
		"interaction_confirm"
	};
	enum InputType : u8 {
		keyDown,
		keyUp
	};
	enum EventType : u8 {
		kDown,
		kUp,
		IGE,
		begin,
		stop,
		checkpoint
	};
	std::string eventKey[6] = {
		"keydown",
		"keyup",
		"ige",
		"start",
		"end",
		"checkpoint"
	};
	enum Input : u8 {
		moveLeft,
		moveRight,
		hold,
		hardDrop,
		rotateCCW,
		rotateCW,
		rotate180,
		softDrop
	};
	std::string key[8] = {
		"moveLeft",
		"moveRight",
		"hold",
		"hardDrop",
		"rotateCCW",
		"rotateCW",
		"rotate180",
		"softDrop"
	};
	/*
	* frame type data
	FULL > data options and some more
	START > data empty
	TARGET> data id diyusi frane 0 type targets
	key up/down > data key subframe
	ige > data type interaction/interaction_confirm data {type=garbage amt x? y? column0-9}sent_frame cid:number
	end > data reaseon topout/winner export{aggregatestats apm pps vsscore}

	*/
	struct User {
		u8 id;
		std::string name;
		json build() const {
			json ret;
			ret["id"] = std::to_string(id);
			ret["username"] = name;
			return ret;
		}
		void init(const u8& id, const std::string& name) {
			this->id = id;
			this->name = name;
		}
		User(const u8& id, const std::string& name) {
			init(id, name);
		}
		User() {}
	};
	struct Stats {
		float apm, pps, vs;
		Stats(const float& a, const float& p, const float& v) : apm(a), pps(p), vs(v) {}
		Stats() : apm(0.0f), pps(0.0f), vs(0.0f) {}
		json build() const {
			json ret;
			ret["apm"] = apm;
			ret["pps"] = pps;
			ret["vsscore"] = vs;
			return ret;
		}
	};
	struct Handling {
		float das;
		float arr;
		u8 sdf;
		json build() const {
			json ret;
			ret["arr"] = arr;
			ret["das"] = das;
			ret["dcd"] = 0;
			ret["sdf"] = sdf;
			ret["may20g"] = false;
			ret["irs"] = "off";
			ret["ihs"] = "off";
			ret["safelock"] = false;
			ret["cancel"] = false;
			return ret;
		}
	};
	struct Options {
		static json buildS2(const User& u, const Handling& h, const u32& seed, const tetris_rule& rule) {
			json ret;
			ret["version"] = 19;
			ret["seed"] = seed;
			ret["g"] = 0;
			ret["countdown"] = true;
			ret["precountdown"] = 5000;
			ret["prestart"] = 1000;
			ret["mission"] = "";
			ret["mission_type"] = "mission_versus";
			ret["zoominto"] = "slow";
			ret["slot_counter1"] = "stopwatch";
			ret["slot_counter2"] = "attack";
			ret["slot_counter3"] = "pieces";
			ret["slot_counter5"] = "vs";
			ret["slot_bar1"] = "impending";
			ret["display_username"] = true;
			ret["hasgarbage"] = true;
			ret["bgmnoreset"] = true;
			ret["neverstopbgm"] = true;
			ret["clutch"] = !!rule.clutch;
			ret["spinbonuses"] = "all-mini+";
			ret["garbagespeed"] = rule.GarbageSpeed;
			ret["garbagecap"] = rule.GarbageCap;
			ret["garbagemultiplier"] = rule.multiplier;
			ret["forfeit_time"] = 150;
			ret["locktime"] = 999999999;
			ret["infinite_movement"] = true;
			ret["allow180"] = true;
			ret["manual_allowed"] = false;
			ret["b2bcharging"] = true;
			ret["b2bcharge_base"] = 3;
			ret["allclear_garbage"] = 5;
			ret["allclear_b2b"] = 1;
			ret["allclear_b2b_sends"] = true;
			ret["allclear_b2b_dupes"] = false;
			ret["nolockout"] = !rule.lockout;
			ret["noextrawidth"] = true;
			ret["garbagespecialbonus"] = true;
			ret["song"] = "none";
			ret["latencymode"] = "low";
			ret["handling"] = h.build();
			ret["gameid"] = u.id;
			ret["username"] = u.name;
			ret["passthrough"] = "limited";
			ret["seed_random"] = false;
			return ret;
		}
		static json buildS1(const User& u, const Handling& h, const u32& seed, const tetris_rule& rule) {
			json ret;
			ret["version"] = 19;
			ret["seed"] = seed;
			ret["g"] = 0;
			ret["countdown"] = true;
			ret["precountdown"] = 5000;
			ret["prestart"] = 1000;
			ret["mission"] = "";
			ret["mission_type"] = "mission_versus";
			ret["zoominto"] = "slow";
			ret["slot_counter1"] = "stopwatch";
			ret["slot_counter2"] = "attack";
			ret["slot_counter3"] = "pieces";
			ret["slot_counter5"] = "vs";
			ret["slot_bar1"] = "impending";
			ret["display_username"] = true;
			ret["hasgarbage"] = true;
			ret["bgmnoreset"] = true;
			ret["neverstopbgm"] = true;
			ret["clutch"] = !!rule.clutch;
			ret["garbagespeed"] = rule.GarbageSpeed;
			ret["garbagecap"] = rule.GarbageCap;
			ret["garbagemultiplier"] = rule.multiplier;
			ret["garbagemargin"] = 0;
			ret["garbageincrease"] = 0;
			ret["forfeit_time"] = 150;
			ret["locktime"] = 999999999;
			ret["infinite_movement"] = true;
			ret["allow180"] = true;
			ret["manual_allowed"] = false;
			ret["b2bchaining"] = true;
			ret["b2bcharge_base"] = 3;
			ret["allclear_b2b_sends"] = true;
			ret["allclear_b2b_dupes"] = false;
			ret["nolockout"] = !rule.lockout;
			ret["noextrawidth"] = true;
			ret["song"] = "none";
			ret["latencymode"] = "low";
			ret["handling"] = h.build();
			ret["gameid"] = u.id;
			ret["username"] = u.name;
			ret["passthrough"] = "limited";
			ret["seed_random"] = false;
			return ret;
		}
	};
	struct Result {
		static json build(const bool& alive) {
			json j;
			j["gameoverreason"] = alive ? "winner" : "garbagesmash";
			return j;
		}
	};
	struct RoundResult {
		static json build(const User& u, const bool& alive, const u32& frames, const Stats& s) {
			json ret = u.build();
			ret["alive"] = alive;
			ret["active"] = true;
			ret["lifetime"] = (u32)(frames * 16.7);
			ret["stats"] = s.build();
			return ret;
		}
	};
	struct Leaderboard {
		static json build(const User& u, const Stats& s, const u16& wins) {
			json ret = u.build();
			ret["active"] = true;
			ret["wins"] = wins;
			ret["stats"] = s.build();
			return ret;
		}
	};
	struct IGEData {
		u32 id;
		IGEType type;
		struct {
			u16 cid;
			u16 amt;
			u8 column;
			json build() const {
				json ret;
				ret["type"] = "garbage";
				ret["amt"] = amt;
				ret["cid"] = cid;
				ret["column"] = column;
				return ret;
			}
		}data;
		json build() const {
			json ret;
			ret["id"] = id;
			ret["type"] = igeKey[type];
			ret["data"] = data.build();
			return ret;
		}
	};
	struct Key {
		float subframe;
		Input input;
		json build() const {
			json ret;
			ret["key"] = key[input];
			ret["subframe"] = subframe;
			return ret;
		}
	};
	union Data {
		IGEData ige;
		Key key;
		json build(const EventType& type) const {
			switch (type) {
			case EventType::IGE:
				return ige.build();
			case EventType::kDown:
			case EventType::kUp:
				return key.build();
			default:
				return json();
			}
		}
	};
	struct Event {
		u32 frame;
		Data data;
		EventType type;
		json build() const {
			json ret;
			ret["frame"] = frame;
			ret["type"] = eventKey[type];
			ret["data"] = data.build(type);
			return ret;
		}
	};
	class EventManager {
	private:
		std::vector<Event> events;
		u16 commits;
	public:
		void push(const u32& frame, const EventType& type, const Data& data) {
			events.push_back({ frame, data, type });
		}
		void commit() {
			++commits;
			events.push_back({ 0, Data(), EventType::checkpoint });
		}
		void discardAll() {
			while (!events.empty() && events.back().type != EventType::checkpoint) {
				events.pop_back();
			}
		}
		void discard() {
			std::vector<Event> ige;
			while (!events.empty()) {
				auto& back = events.back();
				if (back.type == EventType::IGE) {
					ige.push_back(back);
				}
				else if (back.type == EventType::checkpoint) {
					break;
				}
				events.pop_back();
			}
			events.insert(events.end(), ige.rbegin(), ige.rend());
		}
		int size() const {
			return events.size();
		}
		void slice(const bool &clearIGE) {
			if (commits <= 1) {
				if (clearIGE) {
					discardAll();
				}
				else {
					discard();
				}
				return;
			}
			while (!events.empty()) {
				if (events.back().type == EventType::checkpoint) {
					--commits;
					events.pop_back();
					break;
				}
				events.pop_back();
			}
			if (clearIGE) {
				discardAll();
			}
			else {
				discard();
			}
		}
		void reset() {
			events.clear();
			commits = 0;
		}
		std::vector<Event> raw() const {
			std::vector<Event> ret;
			for (const auto& e : events) {
				if (e.type != EventType::checkpoint) {
					ret.push_back(e);
				}
			}
			return ret;
		}
		json build(const u32& frames) const {
			json ret;
			ret["frames"] = frames;
			{
				auto& evts = ret["events"];
				for (const auto& e : events) {
					if (e.type != EventType::checkpoint) {
						evts.push_back(e.build());
					}
				}
			}
			return ret;
		}
	};
	class IGEManager {
	private:
		u32 id;
		u16 cid;
	public:
		std::array<IGEData, 2> raw(const u32& frame, const  u16& amt, const u8& pos) {
			IGEData ret_1{};
			ret_1.id = id++;
			ret_1.type = IGEType::I;
			ret_1.data = { ++cid, amt, pos };
			IGEData ret_2 = ret_1;
			ret_2.id = id++;
			ret_2.type = IGEType::IC;
			return { ret_1, ret_2 };
		}

		void reset() {
			id = 0;
			cid = 0;
		}

		IGEManager() {}
	};
	class KeyManager {
	private:
		int lastFrame;
		double subFrame;
	public:
		void reset() {
			lastFrame = 0;
			subFrame = 0;
		}
		Event raw(const u32& frame, const  InputType& t, const Input& i) {
			if (lastFrame != frame) {
				lastFrame = frame;
				subFrame = 0;
			}
			Event evt{};
			evt.frame = frame;
			evt.type = (EventType)t;
			evt.data.key.input = i;
			evt.data.key.subframe = subFrame;
			subFrame += 0.1;
			return evt;
		}
		KeyManager() {}
	};
	class Player {
	private:
		User self;
		EventManager rp;
		IGEManager ige;
		KeyManager k;
		Handling handling;
		Stats statsAcc;
		u16 wins;
		u32 seed;
		Stats computeAvg(const u32& matches) const {
			Stats s;
			s.apm = statsAcc.apm / matches;
			s.pps = statsAcc.pps / matches;
			s.vs = statsAcc.vs / matches;
			return s;
		}
	public:
		Player(User u, Handling h) : self(u), handling(h), statsAcc{}, wins(0), seed(0) {}
		Player() {}
		json build(const bool& alive, const u32& frames, const Stats& s, const tetris_rule& rule) {
			statsAcc.apm += s.apm;
			statsAcc.pps += s.pps;
			statsAcc.vs += s.vs;
			json ret = RoundResult::build(self, alive, frames, s);
			ret["replay"] = rp.build(frames);
			ret["replay"]["options"] = rule.season == 1 ? Options::buildS1(self, handling, seed, rule) : Options::buildS2(self, handling, seed, rule);
			ret["replay"]["result"] = Result::build(alive);
			if (alive) {
				++wins;
			}
			return ret;
		}
		json buildLb(const u32& matches) const {
			return Leaderboard::build(self, computeAvg(matches), wins);
		}
		int eventSize() const {
			return rp.size();
		}
		void setSeed(const u32& seed) {
			this->seed = seed;
		}
		void start() {
			rp.reset();
			ige.reset();
			k.reset();
			rp.push(0, EventType::begin, {});
			rp.commit();
		}
		void end(const u32& frame) {
			rp.push(frame, EventType::stop, {});
		}
		void move(const u32& frame, const InputType& t, const Input& i) {
			rp.push(frame, (EventType)t, k.raw(frame, t, i).data);
			if (t == InputType::keyUp && i == Input::hardDrop) {
				rp.commit();
			}
		}
		void undo(const bool &clearIGE) {
			rp.slice(clearIGE);
		}
		void recvAttack(const u32& frame, const  u16& amt, const  u8& pos) {
			for (auto& data : ige.raw(frame, amt, pos)) {
				rp.push(frame, EventType::IGE, (Data)data);
			}
		}
		void clearCurrentMove() {
			rp.discard();
		}
	};

	class PlayerManager {
	private:
		std::string filename;
		std::array<Player, 2> p;
		json header;
		json leaderboard;
		json roundReplay;
		u32 matches;
		tetris_rule rule;
		u32 frames;
		bool exported;

		std::string isoTs() {
			using namespace std::chrono;

			auto now = system_clock::now();
			auto now_time_t = system_clock::to_time_t(now);
			auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
			std::tm utc_tm = *std::gmtime(&now_time_t);

			std::ostringstream oss;
			oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%S")
				<< '.' << std::setfill('0') << std::setw(3) << ms.count()
				<< 'Z';

			return oss.str();
		}

		std::string fileTs() {
			std::time_t now = std::time(nullptr);
			std::tm localTime{};
			localtime_s(&localTime, &now);
			std::ostringstream oss;
			oss << std::put_time(&localTime, "%d-%m-%Y %H-%M-%S");
			return oss.str();
		}
	public:
		PlayerManager(const std::array<std::string, 2>& _name, const std::array<Handling, 2>& _handling, const tetris_rule& rule)
			: frames(0), matches(0), rule(rule), exported(true) {
			std::array<User, 2> _user = { User{0, _name[0]}, {1, _name[1]} };
			for (int i = 0; i < 2; ++i) {
				p[i] = Player(_user[i], _handling[i]);
			}
			header["users"] = json::array({ _user[0].build(), _user[1].build() });
			header["ts"] = isoTs();
			header["id"] = nullptr;
			header["gamemode"] = nullptr;
			header["version"] = 1;
			filename = fileTs() + ".ttrm";
		}
		void reportGame(const std::array<Stats, 2>& _stats, const int& winIdx) {
			if (!frames) {
				return;
			}
			json round{};
			for (int i = 0; i < 2; ++i) {
				bool win = winIdx == i;
				p[i].end(frames + win * 5);
				round.push_back(p[i].build(win, frames, _stats[i], rule));
			}
			roundReplay = round;
			frames = 0;
			++matches;
		}
		void recvAttack(const int& idx, const u16& amt, const u8& pos) {
			p[idx].recvAttack(frames, amt, pos);
		}
		void setSeed(const int& idx, const u32& seed) {
			p[idx].setSeed(seed);
		}
		void start() {
			roundReplay.clear();
			frames = 0;
			p[0].start();
			p[1].start();
			exported = false;
		}
		void exportFile() {
			if (exported) {
				return;
			}
			json output;
			{
				std::ifstream ifs(filename);
				std::string read((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

				try {
					output = json::parse(read);
				}
				catch (...) {
					output = header;
				}
			}
			{
				auto& replay = output["replay"];
				replay["leaderboard"] = json::array({ p[0].buildLb(matches), p[1].buildLb(matches) });
				replay["rounds"].push_back(roundReplay);
			}
			{
				std::ofstream ofs(filename);
				if (ofs) {
					ofs << output.dump();
				}
			}
			exported = true;
		}
		void performMove(const int& idx, const InputType& t, const  Input& i) {
			if (!frames) {
				tick(true);
			}
			p[idx].move(frames, t, i);
		}
		int getFrames() const {
			return frames;
		}
		void setFrames(const u32& f) {
			frames = f;
		}
		void tick(const bool& begin = false) {
			if (!begin && !frames) {
				return;
			}
			++frames;
		}
		void undo(const int &idx) {
			p[idx].undo(idx);
		}
		void clearCurrentMove(const int& idx) {
			p[idx].clearCurrentMove();
		}
	};
}
#endif