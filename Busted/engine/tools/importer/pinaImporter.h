#ifndef pina_H
#define pina_H

#include "../../core/all.h"
#include <dae.h>
#include <dae/daeDom.h>
#include <dom/domGeometry.h>
#include <dom/domConstants.h>

class pina : public daeIntegrationObject
{
private:
	std::string		filename;	//Input file
	DAE*			daeObj;		//COLLADA
	domMesh			*mesh;
	
	daeElement*		_element;
	meshObj*		_object;
	input			position;
	input			normal;
	input			texcoord;
	unsigned short	numInputs;

	unsigned int	text_id;

	bool			dlist_on;	//bool for whether or not to use display lists
	
	void			errorReport(daeInt, std::string);

public:
	pina(std::string filename, bool dlist = true);
	~pina(void);

	void setFile(std::string filename)	{ this->filename = filename; };
	std::string getFile(void)			{return this->filename; };
	

	meshObj*		getMesh(void)		{ return _object; };
	
	virtual void pina::fromCOLLADAPostProcess(){}
	virtual void pina::toCOLLADAPostProcess(){}
	virtual void pina::createTo(void *userData){}

	virtual void	load(void);

	domSource*		findSource(domMesh *mesh, std::string id);
	virtual void	updateInputs(domTriangles *triangles, domVertices *vertices);
	virtual void	createFrom(daeElementRef element);
	virtual void	fromCOLLADA(void);
	virtual void	toCOLLADA(void);

	virtual void	save(std::string);

	void SetTextureID( unsigned int i ) { text_id = i; }
	unsigned int GetTextureID() { return text_id;}

	bool			cull_face;	//bool to mark if face should be culled

};

#endif