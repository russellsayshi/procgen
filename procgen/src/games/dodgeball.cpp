#include "../basic-abstract-game.h"
#include "../assetgen.h"
#include <set>
#include <queue>
#include <iostream>
#include <sstream>

const std::string NAME = "dodgeball";

const float COMPLETION_BONUS = 3.0;

const int LAVA_WALL = 1;
const int PLAYER_BALL = 3;
const int ENEMY1 = 11;
const int ENEMY2 = 12;
const int ENEMY3 = 13;
const int ENEMY4 = 14;
const int DOOR = 5;
const int ENEMY1_BALL = 15;
const int ENEMY2_BALL = 16;
const int ENEMY3_BALL = 17;
const int ENEMY4_BALL = 18;
const int DOOR_OPEN = 7;
const int DUST_CLOUD = 8;

const int OOB_WALL = 10;

const int ENEMY_REWARD = 2.0f;
const int NUM_ENEMY_THEMES = 7;

const float ENEMY_VEL = 0.05f;
const float BALL_V_ROT = PI * 0.23f;

class DodgeballGame : public BasicAbstractGame {
  private:

    bool is_enemy(int type) {
        return type == ENEMY1 || type == ENEMY2 || type == ENEMY3 || type == ENEMY4;
    }

    bool is_enemy_ball(int type) {
        return type == ENEMY1_BALL || type == ENEMY2_BALL || type == ENEMY3_BALL || type == ENEMY4_BALL;
    }

  public:
    std::vector<float> ball_score_by_enemy_type;
    std::vector<std::shared_ptr<Entity>> enemies;
    std::vector<bool> enemies_alive;
    int ball_hits[4];

    std::vector<QRectF> rooms;
    bool has_completed_level;
    float min_dim = 0.0f;
    float hard_min_dim = 0.0f;
    float ball_vscale = 0.0f;
    float ball_r = 0.0f;
    int last_fire_time = 0;
    int num_enemies = 0;
    int enemy_fire_delay = 0;
    int ticks = 0;

    DodgeballGame()
        : BasicAbstractGame(NAME) {
        mixrate = .5;

        enemy_fire_delay = 10;

        out_of_bounds_object = OOB_WALL;
    }

    void tell_entity_to_dip(const std::shared_ptr<Entity>& ent) {
	if (ent->will_erase) return;
	ent->will_erase = true;
	if (ent->type == ENEMY1) {
            assert(enemies[0]->type == ENEMY1);
	    assert(enemies_alive[0]);
	    enemies_alive[0] = false;
	} else if (ent->type == ENEMY2) {
            assert(enemies[1]->type == ENEMY2);
	    assert(enemies_alive[1]);
	    enemies_alive[1] = false;
	} else if (ent->type == ENEMY3) {
            assert(enemies[2]->type == ENEMY3);
	    assert(enemies_alive[2]);
	    enemies_alive[2] = false;
	} else if (ent->type == ENEMY4) {
            assert(enemies[3]->type == ENEMY4);
	    assert(enemies_alive[3]);
	    enemies_alive[3] = false;
	}
    }

    void load_background_images() override {
        main_bg_images_ptr = &topdown_backgrounds;
    }

    void asset_for_type(int type, std::vector<std::string> &names) override {
        if (type == PLAYER) {
            names.push_back("misc_assets/character12.png");
        } else if (type == PLAYER_BALL) {
            names.push_back("misc_assets/ball_soccer1.png");
        } else if (type == ENEMY1) {
            names.push_back("misc_assets/character1.png");
            names.push_back("misc_assets/character2.png");
            names.push_back("misc_assets/character3.png");
        } else if (type == ENEMY2) {
            names.push_back("misc_assets/character4.png");
            names.push_back("misc_assets/character5.png");
            names.push_back("misc_assets/character6.png");
        } else if (type == ENEMY3) {
            names.push_back("misc_assets/character7.png");
            names.push_back("misc_assets/character8.png");
            names.push_back("misc_assets/character9.png");
        } else if (type == ENEMY4) {
            names.push_back("misc_assets/character10.png");
            names.push_back("misc_assets/character11.png");
        } else if (type == DOOR) {
            names.push_back("misc_assets/blockRed.png");
        } else if (type == ENEMY1_BALL) {
            names.push_back("misc_assets/ball_soccer2.png");
        } else if (type == ENEMY2_BALL) {
            names.push_back("misc_assets/ball_soccer3.png");
        } else if (type == ENEMY3_BALL) {
            names.push_back("misc_assets/ball_soccer4.png");
        } else if (type == ENEMY4_BALL) {
            names.push_back("misc_assets/ball_soccer5.png");
        } else if (type == DOOR_OPEN) {
            names.push_back("misc_assets/blockGreen.png");
        } else if (type == LAVA_WALL) {
            names.push_back("misc_assets/tileStone_slope2.png");
        } else if (type == OOB_WALL) {
            names.push_back("misc_assets/tileStone_slope2.png");
        } else if (type == DUST_CLOUD) {
            names.push_back("misc_assets/spaceEffect1.png");
            names.push_back("misc_assets/spaceEffect2.png");
            names.push_back("misc_assets/spaceEffect3.png");
            names.push_back("misc_assets/spaceEffect4.png");
            names.push_back("misc_assets/spaceEffect5.png");
            names.push_back("misc_assets/spaceEffect6.png");
            names.push_back("misc_assets/spaceEffect7.png");
            names.push_back("misc_assets/spaceEffect8.png");
            names.push_back("misc_assets/spaceEffect9.png");
        }
    }

    int image_for_type(int type) override {
        if (type == DOOR) {
            return num_enemies == 0 && !has_completed_level ? DOOR_OPEN : DOOR;
        }

        return BasicAbstractGame::image_for_type(type);
    }

    bool will_reflect(int src, int target) override {
        return BasicAbstractGame::will_reflect(src, target) || (is_enemy(src) && (target == LAVA_WALL || target == out_of_bounds_object));
    }

    void handle_agent_collision(const std::shared_ptr<Entity> &obj) override {
        BasicAbstractGame::handle_agent_collision(obj);

	// The below line used to exit in the if block for 
	// ball collisions
	// step_data.done = true;
	
        if (is_enemy(obj->type)) {
		//step_data.done = true;
        } else if (obj->type == ENEMY1_BALL) {
		ball_hits[0]++;
		step_data.reward += ball_score_by_enemy_type[0];
		obj->will_erase = true;
        } else if (obj->type == ENEMY2_BALL) {
		ball_hits[1]++;
		step_data.reward += ball_score_by_enemy_type[1];
		obj->will_erase = true;
        } else if (obj->type == ENEMY3_BALL) {
		ball_hits[2]++;
		step_data.reward += ball_score_by_enemy_type[2];
		obj->will_erase = true;
        } else if (obj->type == ENEMY4_BALL) {
		ball_hits[3]++;
		step_data.reward += ball_score_by_enemy_type[3];
		obj->will_erase = true;
        } else if (obj->type == DOOR) {
            if (num_enemies == 0) {
                //step_data.done = true;
		if (!has_completed_level) {
			step_data.reward += COMPLETION_BONUS;
			has_completed_level = true;
			//step_data.level_complete = true;
		}
            }
        } else if (obj->type == LAVA_WALL) {
		step_data.reward -= 0.5;
            //step_data.done = true;
        }
    }

    void handle_collision(const std::shared_ptr<Entity> &src, const std::shared_ptr<Entity> &target) override {
        if (target->type == PLAYER_BALL) {
            if (src->type == LAVA_WALL) {
		tell_entity_to_dip(target);
            } else if (is_enemy(src->type)) {
                src->health -= 1;
                target->will_erase = true;

                if (src->health <= 0 && !src->will_erase) {
	            tell_entity_to_dip(src);
                    //step_data.reward += ENEMY_REWARD;

                    auto ent = spawn_child(src, DUST_CLOUD, src->rx);
                    ent->vrot = PI / 0.3f;
                    ent->grow_rate = 1.0f / 1.2f;
                    ent->expire_time = 4;
                    ent->alpha_decay = 0.9f;
                    choose_step_random_theme(ent);
                }
            }
        } else if (is_enemy_ball(target->type)) {
            if (src->type == LAVA_WALL) {
                target->will_erase = true;
            }

            // Uncomment to enable ball collisions
            // else if (src->type == PLAYER_BALL) {
            //     src->will_erase = true;
            //     target->will_erase = true;
            // }
        }
    }

    bool use_block_asset(int type) override {
        return BasicAbstractGame::use_block_asset(type) || (type == LAVA_WALL) || (type == DOOR) || (type == DOOR_OPEN);
    }

    void add_room(QRectF room) {
        float rw = room.width();
        float rh = room.height();

        if ((rw >= min_dim || rh >= min_dim) && (rw >= hard_min_dim) && (rh >= hard_min_dim)) {
            rooms.push_back(room);
        }
    }

    void split_room(QRectF room, float thickness) {
        bool will_split_width = rand_gen.rand01() < .5;
        bool choice2 = rand_gen.rand01() < .5;

        if (room.width() < min_dim)
            will_split_width = false;
        if (room.height() < min_dim)
            will_split_width = true;

        float rx = room.x();
        float ry = room.y();
        float rw = room.width();
        float rh = room.height();

        float gap = .25 * (rand_gen.randn(3) + 1);
        float pct = 1 - gap;

        if (!will_split_width) {
            float wy, wh, remy;

            if (choice2) {
                wy = ry;
                remy = ry + pct * rh;
                wh = pct * rh;
            } else {
                wy = ry + (1 - pct) * rh;
                remy = ry;
                wh = pct * rh;
            }

            add_entity_rxy(rx + rw / 2, wy + wh / 2, 0, 0, thickness, wh / 2, LAVA_WALL);

            float nextw = rw / 2 - thickness;

            add_room(QRectF(rx, wy, nextw, wh));
            add_room(QRectF(rx + rw / 2 + thickness, wy, nextw, wh));
            add_room(QRectF(rx, remy, rw, rh - wh));
        } else {
            float wx, ww, remx;

            if (choice2) {
                wx = rx;
                remx = rx + pct * rw;
                ww = pct * rw;
            } else {
                wx = rx + (1 - pct) * rw;
                remx = rx;
                ww = pct * rw;
            }

            add_entity_rxy(wx + ww / 2, ry + rh / 2, 0, 0, ww / 2, thickness, LAVA_WALL);

            float nexth = rh / 2 - thickness;

            add_room(QRectF(wx, ry, ww, nexth));
            add_room(QRectF(wx, ry + rh / 2 + thickness, ww, nexth));
            add_room(QRectF(remx, ry, rw - ww, rh));
        }
    }

    void choose_vel(const std::shared_ptr<Entity> &ent) {
        float vel = ENEMY_VEL * (rand_gen.randn(2) * 2 - 1);

        if (rand_gen.randn(2) == 0) {
            ent->vx = vel;
            ent->vy = 0;
        } else {
            ent->vy = vel;
            ent->vx = 0;
        }

        ent->spawn_time = rand_gen.randn(50) + 25;
    }

    float get_tile_aspect_ratio(const std::shared_ptr<Entity> &ent) override {
        if (ent->type == LAVA_WALL) {
            return ent->rx > ent->ry ? 1 : -1;
        }

        return 0;
    }

    void choose_world_dim() override {
        int world_dim = 20;

        if (options.distribution_mode == MemoryMode) {
            world_dim = 40;
        }

        main_width = world_dim;
        main_height = world_dim;
    }

    void observe() override {
        Game::observe();
        //*(float *)(info_bufs[info_name_to_offset.at("inv2_enemy1")]) = 0.123;
        //*(float *)(info_bufs[info_name_to_offset.at("inv2_enemy2")]) = 0.24;
        //*(float *)(info_bufs[info_name_to_offset.at("inv2_enemy3")]) = 0.124;
        //*(float *)(info_bufs[info_name_to_offset.at("inv2_enemy4")]) = 0.2414;
        *(uint32_t *)(info_bufs[info_name_to_offset.at("inv2_enemy1")]) = ball_hits[0];
        *(uint32_t *)(info_bufs[info_name_to_offset.at("inv2_enemy2")]) = ball_hits[1];
        *(uint32_t *)(info_bufs[info_name_to_offset.at("inv2_enemy3")]) = ball_hits[2];
        *(uint32_t *)(info_bufs[info_name_to_offset.at("inv2_enemy4")]) = ball_hits[3];
        *(uint32_t *)(info_bufs[info_name_to_offset.at("did_reach_exit_door")]) = has_completed_level;
    }

    void game_reset() override {
        BasicAbstractGame::game_reset();
	ticks = 0;
	has_completed_level = false;
	enemies.clear();
	enemies_alive.clear();
	for(int i = 0; i < 4; i++) ball_hits[i] = 0;

        options.center_agent = options.distribution_mode == MemoryMode;

        last_fire_time = 0;

        rooms.clear();
        rooms.push_back(QRectF(0, 0, main_width, main_height));

        int distribution_mode = options.distribution_mode;

        float thickness = 0.3f;
        float enemy_r = .5;
        float exit_r = .75;
        ball_r = .25;
        ball_vscale = .25;
        int num_iterations;
        int max_extra_enemies = 3;

        if (distribution_mode == EasyMode) {
            num_iterations = 2;
            thickness *= 2;
            enemy_r *= 2;
            ball_r *= 2;
            ball_vscale *= 2;
            maxspeed = .75;
            agent->rx = 1;
            agent->ry = 1;
            exit_r *= 2;
        } else if (distribution_mode == HardMode) {
            num_iterations = 4;
            thickness *= 1.5;
            enemy_r *= 1.5;
            ball_r *= 1.5;
            ball_vscale *= 1.5;
            maxspeed = .5;
            agent->rx = .75;
            agent->ry = .75;
        } else if (distribution_mode == ExtremeMode) {
            num_iterations = 8;
            maxspeed = .25;
        } else if (distribution_mode == MemoryMode) {
            num_iterations = 16;
            thickness *= 1.5;
            enemy_r *= 1.5;
            ball_r *= 1.5;
            ball_vscale *= 1.5;
            maxspeed = .5;
            agent->rx = .75;
            agent->ry = .75;
            max_extra_enemies = 16;
        } else {
            fassert(false);
        }

        hard_min_dim = 4 * agent->rx + 2 * thickness + .5;
        min_dim = agent->rx * 8 + .5;

        for (int iteration = 0; iteration < num_iterations; iteration++) {
            if (rooms.size() == 0)
                break;

            int idx = rand_gen.randn((int)(rooms.size()));
            QRectF room = rooms[idx];
            rooms.erase(rooms.begin() + idx);

            split_room(room, thickness);
        }

        float border_r = 0;

        float doorlen = 2 * exit_r;

        int exit_wall_choice = rand_gen.randn(4);

        if (exit_wall_choice == 0) {
            spawn_entity_rxy(doorlen / 2, exit_r, DOOR, 2 * border_r, 2 * border_r, main_width - 4 * border_r, 2 * exit_r);
        } else if (exit_wall_choice == 1) {
            spawn_entity_rxy(doorlen / 2, exit_r, DOOR, 2 * border_r, main_height - 2 * border_r - 2 * exit_r, main_width - 4 * border_r, 2 * exit_r);
        } else if (exit_wall_choice == 2) {
            spawn_entity_rxy(exit_r, doorlen / 2, DOOR, 2 * border_r, 2 * border_r, 2 * exit_r, main_height - 4 * border_r);
        } else if (exit_wall_choice == 3) {
            spawn_entity_rxy(exit_r, doorlen / 2, DOOR, main_width - 2 * border_r - 2 * exit_r, 2 * border_r, 2 * exit_r, main_height - 4 * border_r);
        }

        reposition_agent();

        num_enemies = 4; // set to four // rand_gen.randn(max_extra_enemies + 1) + 4; // TODO russell upped min enemies to four

        int num_enemies4 = num_enemies/4;
        int num_enemies3 = num_enemies4;
        int num_enemies2 = num_enemies/2 - num_enemies4;
        int num_enemies1 = num_enemies - num_enemies4 - num_enemies3 - num_enemies2;

	if(ball_score_by_enemy_type.size() == 0) {
		std::stringstream ss(options.extra_info);
		for (float i; ss >> i;) {
			ball_score_by_enemy_type.push_back(i);
			if (ss.peek() == ',' || ss.peek() == ' ')
				ss.ignore();
		}
		fassert(ball_score_by_enemy_type.size() == 3);
	}

	/*for(int i = 0; i < options.extra_info.size(); i++) {
		char enemy_type_char = options.extra_info[i];
		int enemy_type_int;
		if(enemy_type_char == '1') {
			enemy_type_int = ENEMY1;	
		} else if(enemy_type_char == '2') {
			enemy_type_int = ENEMY2;
		} else if(enemy_type_char == '3') {
			enemy_type_int = ENEMY3;
		} else if(enemy_type_char == '4') {
			enemy_type_int = ENEMY4;
		} else {
			std::cerr << "Unknown enemy type: " << enemy_type_char << std::endl;
			fassert(false);
		}
		spawn_entities(1, enemy_r, enemy_type_int, 0, 0, main_width, main_height);
	}*/
        enemies.push_back(spawn_entity(enemy_r, ENEMY1, 0, 0, main_width, main_height));
        enemies.push_back(spawn_entity(enemy_r, ENEMY2, 0, 0, main_width, main_height));
        enemies.push_back(spawn_entity(enemy_r, ENEMY3, 0, 0, main_width, main_height));
        //enemies.push_back(spawn_entity(enemy_r, ENEMY4, 0, 0, main_width, main_height));
	enemies_alive.push_back(true);
	enemies_alive.push_back(true);
	enemies_alive.push_back(true);
	//enemies_alive.push_back(true);
	assert(enemies.size() == 3);
	assert(enemies_alive.size() == 3);
	/*
        spawn_entities(num_enemies1, enemy_r, ENEMY1, 0, 0, main_width, main_height);
        spawn_entities(num_enemies2, enemy_r, ENEMY2, 0, 0, main_width, main_height);
        spawn_entities(num_enemies3, enemy_r, ENEMY3, 0, 0, main_width, main_height);
        spawn_entities(num_enemies4, enemy_r, ENEMY4, 0, 0, main_width, main_height);
	*/

        int enemy_theme = 1; //rand_gen.randn(2);//NUM_ENEMY_THEMES); TODO RUSSELL fix

        for (auto ent : entities) {
            if (is_enemy(ent->type)) {
                ent->image_theme = enemy_theme;
                ent->health = 1;
                ent->spawn_time = 0;
                ent->fire_time = 10;
                ent->collides_with_entities = true;
                ent->smart_step = true;
                choose_vel(ent);
                ent->face_direction(ent->vx, ent->vy);
            } else if (ent->type == LAVA_WALL) {
                ent->collides_with_entities = true;
            }
        }

        agent->face_direction(1, 0);
    }

    void fire_ball(const std::shared_ptr<Entity> &ent, float vx, float vy) {
	int ball_type;
	if(ent->type == ENEMY1) ball_type = ENEMY1_BALL;
	else if(ent->type == ENEMY2) ball_type = ENEMY2_BALL;
	else if(ent->type == ENEMY3) ball_type = ENEMY3_BALL;
	else if(ent->type == ENEMY4) ball_type = ENEMY4_BALL;
	else {
		ball_type = PLAYER_BALL; // this should never happen
		std::cerr << "Unknown ball type" << std::endl;
	}
        auto new_ball = add_entity(ent->x, ent->y, vx * ball_vscale, vy * ball_vscale, ball_r, ball_type);
        ent->fire_time = cur_time + rand_gen.randn(4)/3.0;
        new_ball->vrot = BALL_V_ROT;
        new_ball->expire_time = 50;
    }

    void game_step() override {
        BasicAbstractGame::game_step();
	ticks++;
	if(ticks >= 250) {
		step_data.done = true;
		step_data.level_complete = true;
	}

        float vx = last_move_action / 3 - 1;
        float vy = last_move_action % 3 - 1;

        agent->face_direction(vx, vy);

        if (special_action == 1 && (cur_time - last_fire_time) >= 7) {
            auto new_ball = add_entity(agent->x, agent->y, vx * ball_vscale, vy * ball_vscale, ball_r, PLAYER_BALL);
            new_ball->collides_with_entities = true;
            new_ball->expire_time = 50;
            new_ball->vrot = BALL_V_ROT;
            last_fire_time = cur_time;
        }

        num_enemies = 0;

        for (int i = (int)(entities.size()) - 1; i >= 0; i--) {
            auto ent = entities[i];

            if (is_enemy(ent->type)) {
                num_enemies++;

                if (ent->spawn_time == 0) {
                    choose_vel(ent);
                } else {
                    ent->spawn_time -= 1;
                }

                bool can_fire = (cur_time - ent->fire_time) >= enemy_fire_delay;

                if (can_fire) {
                    float dx = ent->x - agent->x;
                    float dy = ent->y - agent->y;

                    float bvelx = (ent->x < agent->x ? 1 : -1);
                    float bvely = (ent->y < agent->y ? 1 : -1);

                    if (fabs(dx) < 1) {
                        fire_ball(ent, 0, bvely);
                        ent->vx = 0;
                        ent->vy = bvely * ENEMY_VEL;
                    } else if (fabs(dy) < 1) {
                        fire_ball(ent, bvelx, 0);
                        ent->vx = bvelx * ENEMY_VEL;
                        ent->vy = 0;
                    }

                    // Uncomment to enable diagonal firing
                    // else if (fabs(fabs(dx) - fabs(dy)) < 1) {
                    //     fire_ball(ent, bvelx, bvely);
                    // }
                }

                ent->face_direction(ent->vx, ent->vy);
            } else if (ent->type == PLAYER_BALL || is_enemy_ball(ent->type)) {
                if (ent->x < ent->rx || ent->x > (main_width - ent->rx)) {
                    ent->will_erase = true;
                } else if (ent->y < ent->ry || ent->y > (main_height - ent->ry)) {
                    ent->will_erase = true;
                }
            }
        }

        erase_if_needed();
    }

    void serialize(WriteBuffer *b) override {
        BasicAbstractGame::serialize(b);
        b->write_float(min_dim);
        b->write_float(hard_min_dim);
        b->write_float(ball_vscale);
        b->write_float(ball_r);
        b->write_int(last_fire_time);
        b->write_int(num_enemies);
        b->write_int(enemy_fire_delay);
    }

    void deserialize(ReadBuffer *b) override {
        BasicAbstractGame::deserialize(b);
        min_dim = b->read_float();
        hard_min_dim = b->read_float();
        ball_vscale = b->read_float();
        ball_r = b->read_float();
        last_fire_time = b->read_int();
        num_enemies = b->read_int();
        enemy_fire_delay = b->read_int();
    }
};

REGISTER_GAME(NAME, DodgeballGame);
