/*
BSD 2-Clause License

Copyright (c) 2024, badcoiq
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#ifndef __BQ_XML_H__
#define __BQ_XML_H__

enum class bqXMLXPathTokenType
{
	Slash,
	Double_slash,
	Name,
	Equal,			// price=9.80
	Not_equal,		// price!=9.80
	More,			// price>9.80
	Less,			// price<9.80
	More_eq,		// price>=9.80
	Less_eq,		// price<=9.80
	Apos,
	Number,
	Comma, //,
	Function,
	Function_open,  //(
	Function_close, //)
	Attribute,
	Bit_or,			// //book | //cd
	Sq_open,		// [
	Sq_close,		// ]
	Div,			// 8 div 4
	Mod,			// 5 mod 2
	Add,			// 6 + 4
	Sub,			// 6 - 4
	Mul,			// 6 * 4
	And,			// price>9.00 and price<9.90
	Or,				// price=9.80 or price=9.70,
	Axis_namespace,	//::
	Axis,
	NONE = 0xFFFFFFF
};

enum class bqXMLXPathAxis
{
	Ancestor,			// parent, grandparent, etc.
	Ancestor_or_self,	// parent, grandparent, etc. + current node
	Attribute,
	Child,
	Descendant,			// children, grandchildren, etc.
	Descendant_or_self,	// children, grandchildren, etc. + current node
	Following,
	Following_sibling,
	Namespace,
	Parent,
	Preceding,
	Preceding_sibling,
	Self,
	NONE = 0xFFFFFFF
};

struct bqXMLAttribute
{
	bqXMLAttribute() {}
	bqXMLAttribute(const bqStringA& Name,
		const bqStringA& Value) :
		m_name(Name),
		m_value(Value)
	{}
	~bqXMLAttribute() {}
	bqStringA m_name;
	bqStringA m_value;
};

struct bqXMLNode
{
	bqXMLNode() {}
	bqXMLNode(const bqStringA& Name) :
		m_name(Name)
	{}
	bqXMLNode(const bqXMLNode& node)
	{
		m_name = node.m_name;
		m_text = node.m_text;
		m_attributeList = node.m_attributeList;
		m_nodeList = node.m_nodeList;
	}

	~bqXMLNode()
	{
		clear();
	}

	bqStringA m_name;
	bqStringA m_text;
	bqArray<bqXMLAttribute*> m_attributeList;
	bqArray<bqXMLNode*> m_nodeList;

	void addAttribute(const bqStringA& Name,
		const bqStringA& Value)
	{
		m_attributeList.push_back(new bqXMLAttribute(Name, Value));
	}

	void addAttribute(bqXMLAttribute* a)
	{
		m_attributeList.push_back(a);
	}

	void addNode(bqXMLNode* node)
	{
		m_nodeList.push_back(node);
	}

	bqXMLNode& operator=(const bqXMLNode& node)
	{
		m_name = node.m_name;
		m_text = node.m_text;
		m_attributeList = node.m_attributeList;
		m_nodeList = node.m_nodeList;

		return *this;
	}

	bqXMLAttribute* getAttribute(const bqStringA& Name)
	{
		size_t sz = m_attributeList.size();
		for (size_t i = 0; i < sz; ++i)
		{
			if (m_attributeList[i]->m_name == Name)
			{
				return m_attributeList[i];
			}
		}
		return nullptr;
	}

	void clear()
	{
		m_name.clear();
		m_text.clear();
		size_t sz = m_attributeList.size();
		for (size_t i = 0; i < sz; ++i)
		{
			delete m_attributeList[i];
		}
		sz = m_nodeList.size();
		for (size_t i = 0; i < sz; ++i)
		{
			delete m_nodeList[i];
		}
		m_attributeList.clear();
		m_nodeList.clear();
	}
};

class bqXMLDocument
{
public:
	bqXMLDocument();
	~bqXMLDocument();

	bqXMLNode* GetRootNode();
	void Print();
	const bqStringA& GetText();
	bqArray<bqXMLNode*> SelectNodes(const bqStringA& XPath_expression);
};

#endif

