/***********
Zachary Whaley
gamemogul@gmail.com
--Model Loader--
*
***********/

#include "ModelLoader.h"

#include "../textureload/textureImporter.h"
extern Texture_Manager *pTexture_man;

ModelLoader *pModel_man = new ModelLoader("../Busted/assets/models/default_models.xtx");

//---------------------------
//Constructors/Destructors

	Model::Model() :	mesh(NULL),
						cull_face(true),
						lit(true)
	{}
	Model::~Model(){}

	ModelLoader::ModelLoader(std::string filename)
	{
		//File to open
		this->filename = filename;
		mode = std::ios::in;
	}

	ModelLoader::~ModelLoader(void){}

//---------------------------
std::map<std::string, pina*>* ModelLoader::getModels(void)
{
	return &this->models;
}

//---------------------------
std::map<std::string, Model*>* ModelLoader::getInstances(void)
{
	return &this->instances;
}

//---------------------------
std::map<std::string, Model*>* ModelLoader::getTokens(void)
{
	return &this->tokens;
}

//---------------------------
std::map<int, std::string>*  ModelLoader::getTokenPlace(void)
{
	return &this->token_place_map;
}

//---------------------------
//Open the file for writing
void ModelLoader::open(void)
{
	this->Stat::open();

	//Dump the values in the map and file-list
	this->models.clear();
	this->place_map.clear();

	this->instances.clear();
	this->inst_place_map.clear();

	this->tokens.clear();
	this->token_place_map.clear();

	//Iterate the vector into the map
	for(std::vector<std::string>::iterator fi = parseFile.begin(); fi != parseFile.end(); ++fi )
	{
		//If we're importing another file
		//If we're importing another file
		if((*fi) == "FILE")
		{
			//Do this all over again for this new file...
			copy(*++fi);				//Absorb new file's values
		}
		else if((*fi) == "INST")
		{
			ModelLoader stat(*++fi);
			stat.open();

				Model *obj = new Model;

				if(models[stat.read("mesh")] == NULL)
					continue;

				obj->mesh = models[stat.read("mesh")];

				obj->name = stat.read("name");
				obj->texture = stat.read("texture");

				/*if(stat.read("cull") == "" && stat.read("cull") != "0" && stat.read("cull") != "false" && stat.read("cull") != "FALSE")
					obj->cull_face = true;*/

				if(stat.read("cull") != "" && (stat.read("cull") == "0" || stat.read("cull") == "false" || stat.read("cull") == "FALSE") )
					obj->cull_face = false;

				if(stat.read("lit") != "" && (stat.read("lit") == "0" || stat.read("lit") == "false" || stat.read("lit") == "FALSE") )
					obj->lit = false;

				if(stat.read("posX") != "")
					obj->loc.pos.x = atof(stat.read("posX").c_str());
				if(stat.read("posY") != "")
					obj->loc.pos.y = atof(stat.read("posY").c_str());
				if(stat.read("posZ") != "")
					obj->loc.pos.z = atof(stat.read("posZ").c_str());

				if(stat.read("rotX") != "")
					obj->loc.rot.x = atof(stat.read("rotX").c_str());
				if(stat.read("rotY") != "")
					obj->loc.rot.y = atof(stat.read("rotY").c_str());
				if(stat.read("rotZ") != "")
					obj->loc.rot.z = atof(stat.read("rotZ").c_str());

				if(stat.read("scaleX") != "")
					obj->loc.scale.x = atof(stat.read("scaleX").c_str());
				if(stat.read("scaleY") != "")
					obj->loc.scale.y = atof(stat.read("scaleY").c_str());
				if(stat.read("scaleZ") != "")
					obj->loc.scale.z = atof(stat.read("scaleZ").c_str());

				this->instances[stat.read("name")] = obj;
				this->inst_place_map[int(instances.size())-1] = stat.read("name");

			stat.close();
		}
		else if((*fi) == "TOKEN")
		{
			ModelLoader stat(*++fi);
			stat.open();

				if(stat.read("mesh") != "")
				{
					Model *obj = new Model;
					obj->mesh = new pina(stat.read("mesh"));

					obj->texture = stat.read("texture");

					obj->name = stat.read("name");

					this->tokens[stat.read("name")] = obj;
					this->token_place_map[int(tokens.size())-1] = stat.read("name");
				}

			stat.close();
		}
		else if((*fi) == "MODEL")
		{
			ModelLoader stat(*++fi);
			stat.open();

				if(stat.read("mesh") != "")
				{
					pina *obj = new pina(stat.read("mesh"));

					this->models[stat.read("name")] = obj;
					this->place_map[int(models.size())-1] = stat.read("name");
				}

			stat.close();
		}
	}
}

////---------------------------
////Copy one map into this one from a file
void ModelLoader::copy(std::string File)
{
	ModelLoader *stat = new ModelLoader(File);	//Temp ModelLoader
	stat->open();			//Open and copy

	//Iterate through target map and copy values...
	for(std::map<std::string, std::string>::iterator si = stat->getVars()->begin(); si != stat->getVars()->end(); ++si)
	{
		if((*si).first != "MODEL" && (*si).first != "INST" && (*si).first != "TOKEN")
			this->vars[(*si).first] = (*si).second;
	}
	
	//Copy values from the new map into my local one
	//Iterate through target map and copy values...
	for(std::map<std::string, pina*>::iterator si = stat->getModels()->begin(); si != stat->getModels()->end(); ++si)
	{
		this->models[(*si).first] = (*si).second;
		this->place_map[int(models.size())-1] = (*si).first;
	}
	
	//Copy values from the new map into my local one
	//Iterate through target map and copy values...
	for(std::map<std::string, Model*>::iterator si = stat->getInstances()->begin(); si != stat->getInstances()->end(); ++si)
	{
		this->instances[(*si).first] = (*si).second;
		this->inst_place_map[int(instances.size())-1] = (*si).first;
	}
	
	//Copy values from the new map into my local one
	//Iterate through target map and copy values...
	for(std::map<std::string, Model*>::iterator si = stat->getTokens()->begin(); si != stat->getTokens()->end(); ++si)
	{
		this->tokens[(*si).first] = (*si).second;
		this->token_place_map[int(tokens.size())-1] = (*si).first;
	}

	stat->close();
	delete stat;
}

void ModelLoader::close(void)
{
	this->Stat::close();

	//Dump the values in the sndSet map
	this->models.clear();
	this->place_map.clear();
	this->instances.clear();
	this->inst_place_map.clear();
	this->tokens.clear();
	this->token_place_map.clear();
}