#pragma once
#ifndef REPLAY_H
#define REPLAY_H
#include "json.hpp"
#include "tetrisgame.h"
#include <string>
#include <list>
#include <fstream>

namespace RP {
	using json = nlohmann::json;


	enum {
		FULL,
		KEYDOWN,
		KEYUP,
		START,
		TARGETS,
		IGE,
		IGE_C,
		END
	};
	enum {
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
	typedef struct {
		int frame;
		double subframe;
	} frame_t;
	typedef struct {
		std::list<json> evts;
		bool done;
	} step_t;
	class playerRecord
	{
		int cid = 0;
		int id = 0;
		json info;
		json evt;
		json handling;
		json board;
		json options;
		json endGameStat;
		int totalFrames;
		int undoSteps;
		std::list<step_t> temp_evt;
		std::list<json> ige_reorder;
	public:
		playerRecord() {
			info = {
				{"user",{{"_id","0"},{"username",""}}},
				{"success",false}
			};
			evt["events"] = json::array();
			totalFrames = 0;
			board = json::array();
			int row[10] = { 0 };
			for (int i = 0; i < 40; i++) {
				board.push_back(row);
			}
			undoSteps = 0;
		}
		void reset(int undo) {
			temp_evt.clear();
			ige_reorder.clear();
			undoSteps = undo;
			evt.clear();
			options.clear();
			endGameStat.clear();
			totalFrames = 0;
		}
		// no need to reset
		void setUSer(std::string username, std::string id = "0") {
			info["user"]["username"] = username;
			info["user"]["_id"] = id;
		}
		// no need to reset
		void setHandling(int arr, int das, int sdf) {
			handling = {
				{"arr",arr},
				{"das",das},
				{"dcd",0},
				{"sdf",(sdf == 0)?60:double(200)/sdf},
				{"safelock",true},
				{"cancel",true}
			};
		}
		void setWin(bool win) {
			info["success"] = win;
		}
		void setEndGameStat(int totalFrame, double apm, double pps, double vs) {
			endGameStat["apm"] = apm;
			endGameStat["pps"] = pps;
			endGameStat["vsscore"] = vs;
			evt["frames"] = totalFrame;
		}
		void setOption(unsigned seed, int cap, bool nolockout) {
			options = {
				{"version",15},
				{"seed_random",false},
				{"seed",seed},
				{"g",0},
				{"stock",0},
				{"countdown",true},
				{"countdown_count",3},
				{"countdonw_interval",1000},
				{"precountdown",5000},
				{"prestart",1000},
				{"zoominto","slow"},
				{"slot_counter1","stopwatch"},
				{"slot_counter2","attack"},
				{"slot_counter3","pieces"},
				{"slot_counter4",0},
				{"slot_counter5","vs"},
				{"slot_bar1","impending"},
				{"display_fire",false},
				{"display_username",true},
				{"hasgarbage",true},
				{"neverstopbgm",true},
				{"display_next",true},
				{"display_hold",true},
				{"gmargin",3600},
				{"gincrease",0},
				{"garbagemultiplier",1},
				{"garbagemargin",0},
				{"garbageincrease",0},
				{"garbagecap", cap},
				{"garbagecapincrease",0},
				{"garbagecapmax",40},
				{"bagtype","7bag"},
				{"spinbonuses","T-spins"},
				{"kickset","SRS"},
				{"nextcount",5},
				{"allow_harddrop",true},
				{"display_shadow",true},
				{"locktime",99999},
				{"garbagespeed",1},
				{"forfeit_time",150},
				{"are",0},
				{"lineclear_are",0},
				{"infinitemovement",true},
				{"lockresets",15},
				{"allow180",true},
				{"manual_allowed",false},
				{"b2bchaining",true},
				{"clutch",true},
				{"nolockout", nolockout},
				{"passthrough","zero"},
				{"latencypreference","low"},
				{"noscope",true},
				{"username",info["user"]["username"]},
				{"physical",false},
				{"boardwidth",10},
				{"boardheight",20},
				{"boardbuffer",20},
				{"ghostskin","tetrio"},
				{"boardskin","generic"},
				{"minoskin",{
					{"z","tetrio"},
					{"l","tetrio"},
					{"o","tetrio"},
					{"s","tetrio"},
					{"i","tetrio"},
					{"j","tetrio"},
					{"t","tetrio"},
					{"other","tetrio"},
				}}
			};
		}
		void insertStartEvent() {

		}
		void queueCheck() {
			int totalStoredSteps = 0;
			for (auto it = temp_evt.begin(); it != temp_evt.end(); it++) {
				if (it->done) totalStoredSteps++;
			}
			int excessSteps = max(0, totalStoredSteps - undoSteps);
			while (excessSteps--) {
				flushOneStep();
			}
		}
		void flushOneStep() {
			for (auto it = temp_evt.front().evts.begin(); it != temp_evt.front().evts.end(); it++) {
				if ((*it)["type"] == "ige") {
					ige_reorder.push_back(*it);
					continue;
				}
				while (!ige_reorder.empty() && ige_reorder.front()["frame"] <= (*it)["frame"]) {
					this->evt["events"].push_back(ige_reorder.front());
					ige_reorder.pop_front();
				}
				this->evt["events"].push_back(*it);
			}
			temp_evt.pop_front();
		}
		void flush() {
			while (!temp_evt.empty()) {
				flushOneStep();
			}
			while (!ige_reorder.empty()) {
				this->evt["events"].push_back(ige_reorder.front());
				ige_reorder.pop_front();
			}
		}
		void amendIGEEvt(int frame, atk_t atk) {
			step_t last;
			bool lastExist = false;
			if (!temp_evt.back().done) {
				last = temp_evt.back();
				temp_evt.pop_back();
				lastExist = true;
			}
			json ige;
			//frame = temp_evt.back().evts.back()["frame"];
			ige["frame"] = frame - 1;
			ige["type"] = "ige";
			ige["data"] = {
				{"type","ige"},
				{"data",{
					{"type","interaction"},
					{"sent_frame", frame - 2},
					{"cid",++cid},
					{"data",{
						{"type","garbage"},
						{"amt",atk.atk},
						{"x",0},{"y",0},
						{"column",atk.pos}
						}
					}
					}
				},
				{"frame",frame},
				{"id",++id}
			};
			json ige_c;
			ige_c["frame"] = frame;
			ige_c["type"] = "ige";
			ige_c["data"] = {
				{"type","ige"},
				{"data",{
					{"type","interaction_confirm"},
					{"sent_frame", frame - 1},
					{"cid",cid},
					{"data",{
						{"type","garbage"},
						{"amt",atk.atk},
						{"x",0},{"y",0},
						{"column",atk.pos}
						}
					}
					}
				},
				{"frame",frame},
				{"id",++id}
			};
			temp_evt.back().evts.push_back(ige);
			temp_evt.back().evts.push_back(ige_c);
			if (lastExist) {
				temp_evt.push_back(last);
			}
		}
		void revertHold() {
			if (temp_evt.back().evts.empty()) return;
			temp_evt.back().evts.clear();
		}
		void commitStep() {
			temp_evt.back().done = true;
		}
		void insertTmpEvent(json evt) {
			if (temp_evt.empty() || temp_evt.back().done) {
				temp_evt.push_back({});
				temp_evt.back().done = false;
			}
			temp_evt.back().evts.push_back(evt);
		}
		void undo() {
			if (!temp_evt.back().done) temp_evt.pop_back();
			temp_evt.pop_back();
		}
		void insertEvent(int evt, int frame, void* param, double subframe = 0.0) {
			json event;
			event["frame"] = frame;
			int k;
			atk_t atk;
			bool win;
			switch (evt) {
			case FULL:
				event["type"] = "full";
				event["data"] = {
					{"game", {
						{"board",board},
						{"handling",handling},
						{"playing", true}
						}
					},
					{"options",options }
				};
				break;
			case START:
				event["type"] = "start";
				event["data"] = json::object();
				break;
			case TARGETS:
				event["type"] = "targets";
				event["data"] = {
					{"id","diyusi"},
					{"frame",frame},
					{"type","targets"},
					{"data", json::array({info["user"]["_id"] == 0 ? "1" : "0"})}
				};
				break;
			case KEYDOWN:
				k = *(int*)param;
				event["type"] = "keydown";
				event["data"] = {
					{"key",key[k]},
					{"subframe",0}
				};
				break;
			case KEYUP:
				k = *(int*)param;
				event["type"] = "keyup";
				event["data"] = {
					{"key",key[k]},
					{"subframe",0}
				};
				break;
			case IGE:
				atk = *(atk_t*)param;
				event["type"] = "ige";
				event["data"] = {
					{"type","ige"},
					{"data",{
						{"type","interaction"},
						{"sent_frame", frame},
						{"cid",++cid},
						{"data",{
							{"type","garbage"},
							{"amt",atk.atk},
							{"x",0},{"y",0},
							{"column",atk.pos}
							}
						}
						}
					},
					{"frame",frame + 1},
					{"id",++id}
				};
				break;
			case IGE_C:
				atk = *(atk_t*)param;
				event["type"] = "ige";
				event["data"] = {
					{"type","ige"},
					{"data",{
						{"type","interaction_confirm"},
						{"sent_frame", frame - 1},
						{"cid",cid},
						{"data",{
							{"type","garbage"},
							{"amt",atk.atk},
							{"x",0},{"y",0},
							{"column",atk.pos}
							}
						}
						}
					},
					{"frame",frame},
					{"id",++id}
				};
				break;
			case END:
				win = *(bool*)param;
				event["type"] = "end";
				event["data"] = {
					{"reason",(win) ? "winner" : "topout"},
					{"export",{{"aggregatestats",endGameStat}}}
				};
				break;
			default:
				break;
			}
			if (undoSteps == 0 || (evt == FULL || evt == START || evt == TARGETS || evt == END)) {
				this->evt["events"].push_back(event);
			}
			else {
				if ((evt == IGE || evt == IGE_C) && !temp_evt.empty() && !temp_evt.back().evts.empty() && event["frame"] == temp_evt.back().evts.back()["frame"] && temp_evt.back().evts.back()["type"] == "keyup" && temp_evt.back().evts.back()["data"]["key"] == "hardDrop") {
					FILE* f = fopen("debug.txt", "a");
					event["frame"] = frame - 2;
					fprintf(f, "inserting ige at %d\n", frame);
					fclose(f);
					RP::json last = temp_evt.back().evts.back();
					temp_evt.back().evts.pop_back();
					RP::json last2 = temp_evt.back().evts.back();
					temp_evt.back().evts.pop_back();
					insertTmpEvent(event);
					insertTmpEvent(last2);
					insertTmpEvent(last);
				}
				else {
					insertTmpEvent(event);
				}
			}
			if (evt == IGE) {
				insertEvent(IGE_C, frame, param);
			}
		}
		json getInfo() {
			return info;
		}
		json getEvents() {
			return evt;
		}
	};
	class GameRecord
	{
		json game;
	public:
		GameRecord() {
			game["board"] = json::array();
			game["replays"] = json::array();
		}
		void insertGame(playerRecord r) {
			game["board"].push_back(r.getInfo());
			game["replays"].push_back(r.getEvents());
		}
		void reset(){
			game.clear();
			game["board"].clear();
			game["replays"].clear();
		}
		json getGame() {
			return game;
		}
	};
	class Replay
	{
	std::string filename;
	json r;
	public:
	Replay(std::string filename, std::string timestamp) {
		this->filename = filename;
		r["ts"] = timestamp;
		r["ismulti"] = true;
		r["data"] = json::array();
		r["endcontext"] = json::array();
	}
	void reset(std::string filename, std::string timestamp) {
		this->filename = filename;
		r["ts"] = timestamp;
		r["data"].clear();
	}
	void toFile() {
		std::ofstream f;
		f.open(this->filename + ".ttrm");
		f << r;;
		f.close();
	}
	void setUser(std::string user1, std::string user2, std::string user1ID = "0", std::string user2ID = "0") {
		json juser1 = {
			{ "user",{{"_id",user1ID}, {"username",user1}} },
			{ "active",true }
		};
		json juser2 = {
			{ "user",{{"_id",user2ID}, {"username",user2}} },
			{ "active",true }
		};
		r["endcontext"].push_back(juser1);
		r["endcontext"].push_back(juser2);

	}
	void insertGame(GameRecord g) {
		r["data"].push_back(g.getGame());
	}
	};
}
#endif
