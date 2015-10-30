#pragma once
#include "stdafx.h"
#include "DungeonGenerator.h"
#include "stdlib.h"
#include "time.h"
#include <utility>

DungeonGenerator::DungeonGenerator(){
	srand(time(NULL));
}

DungeonGenerator::~DungeonGenerator(){

}

Dungeon* DungeonGenerator::GenerateDungeon(int height, int width){
	Dungeon* d = new Dungeon(height, width);
	dungeonHeight = height;
	dungeonWidth = width;
	exitRoom = nullptr;
	int level = 1;
	while (level < 11){
		d->setLevel(level, GenerateLayer(level));
	}


	return d;

}

DungeonLayer* DungeonGenerator::GenerateLayer(int layer){
	DungeonLayer* dlevel = new DungeonLayer();
	std::vector<std::vector<Room*>> levelArray;
	std::vector<Room*> allRooms;
	std::pair<int, int> startRoom;
	std::pair<int, int> endRoom;
	startRoom.first = RandomNumberGenerator(0, dungeonWidth);
	startRoom.second = RandomNumberGenerator(0, dungeonHeight);
	endRoom.first = RandomNumberGenerator(0, dungeonWidth);
	endRoom.second = RandomNumberGenerator(0, dungeonHeight);
	levelArray.resize(dungeonHeight);
	for (int i = 0; i < dungeonHeight; i++){
		levelArray[i].resize(dungeonWidth);
	}
	for (int i = 0; i < dungeonHeight; i++){
		for (int j = 0; j < dungeonWidth; j++){
			levelArray[i][j] = new Room();
			allRooms.push_back(levelArray[i][j]);
		}
	}
	dlevel->setEnd(levelArray[endRoom.first][endRoom.second]);
	dlevel->setStart(levelArray[startRoom.first][startRoom.second]);
	dlevel->setRooms(allRooms);
	return dlevel;
}



int DungeonGenerator::RandomNumberGenerator(int lowest, int highest){
	return (lowest + rand() % (highest - lowest + 1));

}