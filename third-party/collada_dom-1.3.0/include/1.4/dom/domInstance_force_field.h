/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the SCEA Shared Source License, Version 1.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at:
 * http://research.scea.com/scea_shared_source_license.html
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License 
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or 
 * implied. See the License for the specific language governing permissions and limitations under the 
 * License. 
 */
#ifndef __domInstance_force_field_h__
#define __domInstance_force_field_h__

#include <dae/daeDocument.h>
#include <dom/domTypes.h>
#include <dom/domElements.h>

#include <dom/domInstanceWithExtra.h>

/**
 * The instance_force_field element declares the instantiation of a COLLADA
 * force_field resource.
 */
class domInstance_force_field : public daeElement, public domInstanceWithExtra_complexType
{
public:
	COLLADA_TYPE::TypeEnum getElementType() const { return COLLADA_TYPE::INSTANCE_FORCE_FIELD; }

	/**
	 * Gets the url attribute.
	 * @return Returns a xsAnyURI reference of the url attribute.
	 */
	xsAnyURI &getUrl() { return attrUrl; }
	/**
	 * Gets the url attribute.
	 * @return Returns a constant xsAnyURI reference of the url attribute.
	 */
	const xsAnyURI &getUrl() const { return attrUrl; }
	/**
	 * Sets the url attribute.
	 * @param atUrl The new value for the url attribute.
	 */
	void setUrl( const xsAnyURI &atUrl ) { attrUrl = atUrl; _validAttributeArray[0] = true; }

	/**
	 * Gets the sid attribute.
	 * @return Returns a xsNCName of the sid attribute.
	 */
	xsNCName getSid() const { return attrSid; }
	/**
	 * Sets the sid attribute.
	 * @param atSid The new value for the sid attribute.
	 */
	void setSid( xsNCName atSid ) { *(daeStringRef*)&attrSid = atSid; _validAttributeArray[1] = true; }

	/**
	 * Gets the name attribute.
	 * @return Returns a xsNCName of the name attribute.
	 */
	xsNCName getName() const { return attrName; }
	/**
	 * Sets the name attribute.
	 * @param atName The new value for the name attribute.
	 */
	void setName( xsNCName atName ) { *(daeStringRef*)&attrName = atName; _validAttributeArray[2] = true; }

protected:
	/**
	 * Constructor
	 */
	domInstance_force_field() {}
	/**
	 * Destructor
	 */
	virtual ~domInstance_force_field() {}
	/**
	 * Copy Constructor
	 */
	domInstance_force_field( const domInstance_force_field &cpy ) : daeElement(), domInstanceWithExtra_complexType() { (void)cpy; }
	/**
	 * Overloaded assignment operator
	 */
	virtual domInstance_force_field &operator=( const domInstance_force_field &cpy ) { (void)cpy; return *this; }

public: // STATIC METHODS
	/**
	 * Creates an instance of this class and returns a daeElementRef referencing it.
	 * @param bytes The size allocated for this instance.
	 * @return a daeElementRef referencing an instance of this object.
	 */
	static DLLSPEC daeElementRef create(daeInt bytes);
	/**
	 * Creates a daeMetaElement object that describes this element in the meta object reflection framework.
	 * If a daeMetaElement already exists it will return that instead of creating a new one. 
	 * @return A daeMetaElement describing this COLLADA element.
	 */
	static DLLSPEC daeMetaElement* registerElement();

public: // STATIC MEMBERS
	/**
	 * The daeMetaElement that describes this element in the meta object reflection framework.
	 */
	static DLLSPEC daeMetaElement* _Meta;
};


#endif
