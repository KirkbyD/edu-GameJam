#pragma once
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>

class iSerializable {
public:
	virtual ~iSerializable() = default;
	virtual bool serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) = 0;
	virtual bool deserialize(const rapidjson::Document* document) = 0;
};

class cJSONUtility {
public:
	static rapidjson::Document open_document(std::string filename) {
		std::ifstream ifs(filename);

		if (!ifs.is_open()) {
			return nullptr;
		}

		rapidjson::IStreamWrapper isw(ifs);
		rapidjson::Document document;
		document.ParseStream(isw);

		return document;
	}

	static bool serialize(iSerializable* object, rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer)	{
		return object->serialize(writer);
	}

	static bool deserialize(iSerializable* object, const rapidjson::Document* document)	{
		return object->deserialize(document);
	}
};

