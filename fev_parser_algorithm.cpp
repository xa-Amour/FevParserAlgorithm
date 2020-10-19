#include "stdafx.h"
#include <iostream>
#include "fmod.hpp"
#include "fmod.h"
#include <fmod_event.hpp>
#include <string>
#include <fstream>
#include <vector>
#pragma comment(lib, "ws2_32.lib")

using namespace std;


int main()
{
	ofstream ofile("outputfile");

	//初始化 init
	FMOD::System        *system;
	FMOD::EventSystem	*eventsystem;
	FMOD::EventSystem_Create(&eventsystem);
	//std::unordered_map<std::string, std::string>  mEncryptionKey;
	eventsystem->getSystemObject(&system);
	eventsystem->init(32, FMOD_INIT_NORMAL, 0, FMOD_EVENT_INIT_NORMAL);

	//load fev
	eventsystem->load("inputfile.fev", 0, 0);

	//使用EventProject处理fev文件
	int numProj = 0;
	FMOD::EventProject* ep;
	FMOD_RESULT result = eventsystem->getNumProjects(&numProj);
	//std::cout << numProj<<std::endl;

	struct EGroup//加入该文件名字名字，这里只有一个文件
	{
		string path;
		FMOD::EventGroup* eGroup;
	};
	std::vector<EGroup> groups;
	for (int i = 0; i < numProj; ++i)
	{
		FMOD::EventProject* ep;
		result = eventsystem->getProjectByIndex(i, &ep);
		if (result != FMOD_OK || ep == nullptr)
			continue;

		int numGroup = 0;
		result = ep->getNumGroups(&numGroup);
		if (result != FMOD_OK || numGroup == 0)
			continue;

		FMOD_EVENT_PROJECTINFO info;
		result = ep->getInfo(&info);
		if (result != FMOD_OK)
			continue;

		for (int j = 0; j < numGroup; ++j)
		{
			EGroup eg;
			eg.path = info.name;
			result = ep->getGroupByIndex(j, false, &eg.eGroup);
			if (result == FMOD_OK && eg.eGroup != nullptr)
				groups.emplace_back(eg);
		}
	}

	//处理eGroup
	char* eventName = nullptr;
	char* groupName = nullptr;
	while (groups.size() > 0)
	{
		EGroup curEG = groups.back();
		groups.pop_back();            //取出一个

						             //处理eGroup
		result = curEG.eGroup->getInfo(nullptr, &groupName);
		if (groupName == nullptr)
			cout << "no data" << endl;
		int numGroups = 0;
		result = curEG.eGroup->getNumGroups(&numGroups);

		if (result == FMOD_OK && numGroups > 0)
		{
			for (int i = 0; i < numGroups; ++i)
			{
				EGroup eg;
				eg.path = curEG.path + "/" + string(groupName);
				result = curEG.eGroup->getGroupByIndex(i, false, &eg.eGroup);
				if (result == FMOD_OK && eg.eGroup != nullptr)
					groups.emplace_back(eg);
			}
		}

		int numEvents = 0;
		result = curEG.eGroup->getNumEvents(&numEvents);
		if (result == FMOD_OK && numEvents > 0)
		{
			for (int i = 0; i < numEvents; ++i)
			{
				FMOD::Event* event = nullptr;
				result = curEG.eGroup->getEventByIndex(i, FMOD_EVENT_INFOONLY, &event);
				if (result != FMOD_OK || event == nullptr)
					cout << "no event" << endl;

				result = event->getInfo(nullptr, &eventName, nullptr);
				if (result == FMOD_OK && eventName != nullptr)
				{
					string fullName = curEG.path + "/" + string(groupName) + "/" + string(eventName) + "\n";
					ofile << fullName;
					cout << fullName << endl;
				}
				event->release(true);
			}
		}

	}
	ofile.close();
}

