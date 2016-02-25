#pragma once
#include "stdafx.h"
#include "Game.h"
#include <algorithm>




Game::Game()
{
	Utils::PrintLine("This story is all about you. What was your name again?");
	std::string name = Utils::ReadString();
	player = new Player(name);
	Utils::LoadPlayer(name, player);
	playing = true;
	Utils::PrintLine("What kind of dungeon do you want? small(4-6), medium(8-10) or large(12-14)? Info: Typo's or nothing will make a medium dungeon");
	std::string size = Utils::ReadString();
	Utils::PrintLine("What difficulty do you want? normal, hard or insane");
	std::string difficulty = Utils::ReadString();
	generateDungeon(size, difficulty);
	dungeon->loadLevel(1);
	player->setCurrentRoom(dungeon->getFirstRoom());
	dungeon->getFirstRoom()->Visited();
	startGame();
}

void Game::generateDungeon(std::string size, std::string difficulty){
	DungeonGenerator* dgen = new DungeonGenerator();
	dungeon = dgen->GenerateDungeon(size, difficulty);
	delete(dgen);
}

void Game::startGame() {
	Utils::cClear();
	while (playing) {
		//fighting loop
		while (fighting){
			if (player->getCurrentRoom()->hasEnemies()){
				Utils::Print("\n" + player->getCurrentRoom()->getEnemyInfo(true));
				Utils::PrintLine(player->getStatus());
				Utils::PrintLine("\nWhat do you want to do?\nAttack | Run | Use Potion | Use Item");
				std::string choice = Utils::ReadString();
				handleInput(choice);
				Utils::cClear();
			}
			else {
				Utils::PrintLine("You defeated the enemy.");
				fighting = false;
			}
			

		}
		// print map
		Utils::Print(dungeon->getCurrentLayer()->getDungeonMap(cheat, player->getCurrentRoom()));
		if (cheat){
			cheat = false;
		}
		if (affected) {
			affected = false;
			Utils::PrintLine(effect);
		}
		if (showStats){
			showStats = false;
			Utils::PrintLine(player->getStatus() + "\n");
		}
		if (showInventory) {
			showInventory = false;
			std::string temp = "";
			if (player->getSword() != nullptr || player->getShield() != nullptr){
				temp += "Equiped:\n";
				int count = 0;
				if (player->getSword() != nullptr){
					count++;
					temp += "[1]" + player->getSword()->getText() + ": " + std::to_string(player->getSword()->getEquipPower()) + "\n";
				}
				if (player->getShield() != nullptr){
					count++;
					temp += "[2]" + player->getShield()->getText() + ": " + std::to_string(player->getShield()->getEquipPower()) + "\n";
				}
				temp += "\n";
			}
			
			if (player->getItems().size() == 0){
				temp += "You don't have any items right now.";
			}
			else {
				temp += "Inventory: \n";
				for (int i = 0; i < player->getItems().size(); i++){
					temp += "[" + std::to_string(i + 1) + "] " + std::string(player->getItems()[i]->getText()) + "\n";
				}
			}
			Utils::PrintLine(temp + "\n");
		}
		// Print all actions
		if (player->getCurrentRoom() == dungeon->getFirstRoom()) {
			Utils::PrintLine("This is the first room of this layer.");
		} else if (player->getCurrentRoom() == dungeon->getLastRoom()) {
			Utils::PrintLine("You have found the last room in this layer! Do you wish to go down, further into the dungeon?");
		}
		Utils::Print(player->getCurrentRoom()->getRoomInfo());
		player->getCurrentRoom()->checkForTraps(player);
		

		
		actions = "|";
		for (std::string actionString : player->getCurrentRoom()->getPossibleActions()) {
			if (actionString != "") {
				actions += "  " + actionString + "  |";
			}
		}
		if (player->getCurrentRoom() == dungeon->getLastRoom()){
			actions += "  Down |";
		}
		else if (player->getCurrentRoom() == dungeon->getFirstRoom() && dungeon->getLevel() != 1){
			actions += "  Up |";
		}
		actions += "  Quit |";
		Utils::PrintLine(actions);
		Utils::PrintLine("Please enter your choice below.");
		std::string choice = Utils::ReadString();
		handleInput(choice);
		Utils::cClear();


	}

}

void Game::handleInput(std::string input) {
	std::transform(input.begin(), input.end(), input.begin(), ::tolower);
	if (input == std::string("run")) {
		if (fighting){
			fighting = false;
			Utils::cClear();
		}
		else {
			map<Direction, Room*>::iterator it;
			map<Direction, Room*> rooms = player->getCurrentRoom()->getAdjacentRooms();

			Utils::Print("There are exits in the following directions: ");
			for (it = rooms.begin(); it != rooms.end(); it++)
			{
				Utils::Print(it->second->ToString(it->first) + ", ");
			}

			Utils::PrintLine("if you wish to travel type one of these directions below.");
			std::string direction = Utils::ReadString();
			player->goDirection(direction);
		}
	}
	else if (input == std::string("endgame")) {
		endGame();
	}
	else if (input == std::string("cheat")){
		cheat = true;
	}
	else if (input == std::string("fight")){
		if (player->getCurrentRoom()->hasEnemies()){
			fighting = true;
		}
		else {
			Utils::PrintLine("There are no enemies to fight, please select another option");
			handleInput(Utils::ReadString());
		}
	}
	else if (input == std::string("attack")){
		// currentroom has more enemies

		if (player->getCurrentRoom()->hasEnemies() > 1 && fighting){
			Utils::PrintLine("Which monster do you wanna attack?");
			int monsterno = std::stoi(Utils::ReadString());

			while (!player->getCurrentRoom()->AttackEnemy(monsterno, player)){
				Utils::PrintLine("That monster number isn't existing, Try again.");
				monsterno = std::stoi(Utils::ReadString());
			}
		}
		else { // only 1;

			player->getCurrentRoom()->AttackEnemy(0, player);
		}
		player->getCurrentRoom()->EnemiesAttack(player);
	}
	else if (input == std::string("use")){
		int item = handleItemInput(Utils::ReadString());
		if (player->getItems().at(item - 1) != NULL) {
			effect = player->useItem(player->getItems().at(item - 1));
			affect();
		}
	}
	else if (input == std::string("equip")){
		int item = handleItemInput(Utils::ReadString());
		if (player->getItems()[item - 1] != nullptr){
			Equipable* e = (Equipable*)player->getItems()[item - 1];
			e->equip(player); //TODO fix deletion of this item, just removing it from the vector, nothing more.
			std::swap(player->getItems()[item - 1], player->getItems().back());
			player->getItems().pop_back();
		}
	}
	else if (input == std::string("unequip")){
		int item = handleItemInput(Utils::ReadString());
		player->unEquipItem(item);
	}
	else if (input == std::string("inv")){
		showInventory = true;
	}
	else if (input == std::string("stats")){
		showStats = true;
	}
	else if (input == std::string("quit")){
		//save player stats and progress on name, in name.txt
		Utils::PrintLine("Are you sure your wanna quit? Y/N");
		std::string q = Utils::ReadString();
		std::transform(q.begin(), q.end(), q.begin(), ::tolower);
		if (q == "y"){
			playing = false;
		}
	}
	else if(input == std::string("rest")){
		player->heal(5);
		showStats = true;
	}
	else if (input == std::string("item")) {
		player->getCurrentRoom()->checkForItems();
		handleInput(Utils::ReadString());
	}
	else if (input == std::string("take")) {
		player->takeItem(player->getCurrentRoom()->getItem());
	}
	else if (input == std::string("leave")) {
		
	}
	else if (input == std::string("down")){
		if (player->getCurrentRoom() == dungeon->getLastRoom() && player->getCurrentRoom()->hasEnemies() ==0){
			dungeon->loadNextLevel();
			player->setCurrentRoom(dungeon->getFirstRoom());
			player->getCurrentRoom()->Visited();
		}

	}
	else if (input == std::string("up")){
		if (player->getCurrentRoom() == dungeon->getFirstRoom() && dungeon->getLevel() > 1){
			dungeon->loadLevel(dungeon->getLevel()-1);
			player->setCurrentRoom(dungeon->getLastRoom());
		}

	}
	else if (input == std::string("save")){
		player->save();
	}
	else {
		Utils::PrintLine("The input " + input + " is not known, try again.");
		handleInput(Utils::ReadString());
	}
}

int Game::handleItemInput(std::string input) {
	if (int i = atoi(input.c_str())) {
		return i;
	}
}

void Game::endGame() {
	playing = false;

}

void Game::affect() {
	affected = true;
	showStats = true;
}

Game::~Game()
{
	delete dungeon;
	delete player;
}




