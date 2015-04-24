/***********
Zachary Whaley
gamemogul@gmail.com
--Model Loader--
*
***********/

#ifndef _MODELLOADER_H_
#define _MODELLOADER_H_

#include "../../core/stat_manager/stat_manager.h"
#include "../../core/classes.h"
#include "pinaImporter.h"

class Model
{
public:
	std::string name;
	pina *mesh;
	std::string texture;

	locator loc;

	bool cull_face;
	bool lit;

	Model();
	~Model();
};

class ModelLoader : public Stat
{
private:
	std::map<std::string, pina*>	models;
	std::map<int, std::string>		place_map;

	std::map<std::string, Model*>	instances;
	std::map<int, std::string>		inst_place_map;

	std::map<std::string, Model*>	tokens;
	std::map<int, std::string>		token_place_map;
public:

	std::map<std::string, pina*>* getModels(void);
	std::map<std::string, Model*>* getInstances(void);
	std::map<std::string, Model*>* getTokens(void);

	std::map<int, std::string>* getTokenPlace(void);

	virtual void open(void);

	virtual void copy(std::string);

	virtual void close(void);

	ModelLoader(std::string);
	virtual ~ModelLoader(void);

};

#endif