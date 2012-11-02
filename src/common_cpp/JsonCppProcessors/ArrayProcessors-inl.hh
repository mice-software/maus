/* This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <vector>

#include "src/common_cpp/Utils/JsonWrapper.hh"
#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolver.hh"

namespace MAUS {

template <class ArrayContents>
PointerArrayProcessor<ArrayContents>::PointerArrayProcessor
                            (ProcessorBase<ArrayContents>* contents_processor)
                                                   : _proc(contents_processor) {
}

template <class ArrayContents>
PointerArrayProcessor<ArrayContents>::~PointerArrayProcessor<ArrayContents>() {
    if (_proc != NULL) {
        delete _proc;
        _proc = NULL;
    }
}

template <class ArrayContents>
std::vector<ArrayContents*>* PointerArrayProcessor<ArrayContents>::JsonToCpp
                                               (const Json::Value& json_array) {
    if (!json_array.isConvertibleTo(Json::arrayValue)) {
        // no memory allocated yet...
        throw(Squeal(Squeal::recoverable,
                    "Failed to resolve Json::Value of type "+
                    JsonWrapper::ValueTypeToString(json_array.type())+
                    " to array",
                    "PointerArrayProcessor::JsonToCpp()"
                    ) );
    }
    std::vector<ArrayContents*>* vec = new std::vector<ArrayContents*>(json_array.size());
    for (size_t i = 0; i < json_array.size(); ++i) {
        try {
            // allocate the vector
            if (json_array[i].type() == Json::nullValue) {
                (*vec)[i] = NULL;
            } else {
                ArrayContents* data = _proc->JsonToCpp(json_array[i]);
                (*vec)[i] = data;
                ReferenceResolver::ChildTypedJsonToCppResolver<ArrayContents>::AddData(JsonWrapper::GetPath(json_array[i]), (*vec)[i]);
            }
        } catch(Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            for (size_t j = 0; j < vec->size(); ++j) {
                if ((*vec)[j] != NULL) {
                    delete (*vec)[j];
                }
            }
            delete vec;
            throw squee;
        }
    }
    return vec;
}

template <class ArrayContents>
Json::Value* PointerArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents*>& cpp_array) {
    return CppToJson(cpp_array, "");
}

template <class ArrayContents>
Json::Value* PointerArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents*>& cpp_array,
                                std::string path) {
    Json::Value* json_array = new Json::Value(Json::arrayValue);
    JsonWrapper::SetPath(*json_array, path);
    json_array->resize(cpp_array.size());

    for (size_t i = 0; i < cpp_array.size(); ++i) {
        try {
            Json::Value* data = NULL;
            if (cpp_array[i] == NULL) {
                data = new Json::Value();  // that is a NULL value
            } else {
                data = _proc->CppToJson(*cpp_array[i], GetPath(path, i));
            }
            (*json_array)[i] = *data; // json copies memory here but not path
            JsonWrapper::SetPath((*json_array)[i], GetPath(path, i));
            delete data; // so we need to clean up here
            ReferenceResolver::TypedCppToJsonResolver<ArrayContents>::AddData(cpp_array[i], JsonWrapper::GetPath((*json_array)[i]));
        } catch(Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            delete json_array;
            throw squee;
        }
    }
    return json_array;
}

template <class ArrayContents>
std::string PointerArrayProcessor<ArrayContents>::GetPath
                                              (std::string path, size_t index) {
    return path+"/"+STLUtils::ToString(index);
}
///////////

template <class ArrayContents>
ValueArrayProcessor<ArrayContents>::ValueArrayProcessor
                              (ProcessorBase<ArrayContents>* contents_processor)
                                                   : _proc(contents_processor) {
}

template <class ArrayContents>
ValueArrayProcessor<ArrayContents>::~ValueArrayProcessor<ArrayContents>() {
    if (_proc != NULL) {
        delete _proc;
        _proc = NULL;
    }
}


template <class ArrayContents>
std::vector<ArrayContents>* ValueArrayProcessor<ArrayContents>::JsonToCpp
                                               (const Json::Value& json_array) {
    if (!json_array.isConvertibleTo(Json::arrayValue)) {
        // no memory allocated yet...
        throw(Squeal(Squeal::recoverable,
                    "Failed to resolve Json::Value of type "+
                    JsonWrapper::ValueTypeToString(json_array.type())+
                    " to array",
                    "ValueArrayProcessor::JsonToCpp()"
                    ) );
    }
    std::vector<ArrayContents>* vec
                            = new std::vector<ArrayContents>(json_array.size());

    for (size_t i = 0; i < json_array.size(); ++i) {
        try {
            // allocate the vector
            ArrayContents* data = _proc->JsonToCpp(json_array[i]);
            (*vec)[i] = *data;
            delete data;
        } catch(Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            delete vec;
            throw squee;
        }
    }
    return vec;
}


template <class ArrayContents>
Json::Value* ValueArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents>& cpp_array) {
    return CppToJson(cpp_array, "");
}

template <class ArrayContents>
Json::Value* ValueArrayProcessor<ArrayContents>::
                      CppToJson(const std::vector<ArrayContents>& cpp_array,
                                std::string path) {
    Json::Value* json_array = new Json::Value(Json::arrayValue);
    JsonWrapper::SetPath(*json_array, path);
    json_array->resize(cpp_array.size());

    for (size_t i = 0; i < cpp_array.size(); ++i) {
        try {
            Json::Value* data = _proc->CppToJson(cpp_array[i],
                                                 GetPath(path, i));
            (*json_array)[i] = *data; // json copies memory but not path
            JsonWrapper::SetPath((*json_array)[i], GetPath(path, i));
            delete data; // so we need to clean up here
        } catch(Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            delete json_array;
            throw squee;
        }
    }
    return json_array;
}

template <class ArrayContents>
std::string ValueArrayProcessor<ArrayContents>::GetPath
                                              (std::string path, size_t index) {
    return path+"/"+STLUtils::ToString(index);
}

////////////////////////////
template <class ArrayContents>
std::vector<ArrayContents*>* ReferenceArrayProcessor<ArrayContents>::JsonToCpp
                                               (const Json::Value& json_array) {
    if (!json_array.isConvertibleTo(Json::arrayValue)) {
        // no memory allocated yet...
        throw(Squeal(Squeal::recoverable,
                    "Failed to resolve Json::Value of type "+
                    JsonWrapper::ValueTypeToString(json_array.type())+
                    " to array",
                    "ValueArrayProcessor::JsonToCpp()"
                    ) );
    }
    std::vector<ArrayContents*>* vec
                     = new std::vector<ArrayContents*>(json_array.size(), NULL);

    for (size_t i = 0; i < json_array.size(); ++i) {
        try {
            using namespace ReferenceResolver;
            using namespace ReferenceResolver::JsonToCpp;
            std::cerr << json_array[i] << std::endl;
            std::string data_path = JsonWrapper::GetProperty
                   (json_array[i], "$ref", JsonWrapper::stringValue).asString();
            // allocate the vector
            VectorResolver<ArrayContents>* res =
                          new VectorResolver<ArrayContents>(data_path, *vec, i);
            JsonToCppManager::GetInstance().AddReference(res);
        } catch(Squeal squee) {
            // if there's a problem, clean up before rethrowing the exception
            delete vec;
            throw squee;
        }
    }
    return vec;
}

template <class ArrayContents>
Json::Value* ReferenceArrayProcessor<ArrayContents>::CppToJson(
                           const std::vector<ArrayContents*>& cpp_array,
                           std::string path) {
    Json::Value* array = new Json::Value(Json::arrayValue);
    JsonWrapper::SetPath(*array, path);
    array->resize(cpp_array.size());
    for (size_t i = 0; i < cpp_array.size(); ++i) {
        (*array)[i] = Json::Value();
        JsonWrapper::SetPath((*array)[i], path);
        JsonWrapper::AppendPath((*array)[i], i);
        std::cerr << "Setting path " << JsonWrapper::GetPath((*array)[i]) << " for CPP object " << cpp_array[i] << std::endl;
        ReferenceResolver::TypedCppToJsonResolver<ArrayContents>* res = new ReferenceResolver::TypedCppToJsonResolver<ArrayContents>(cpp_array[i], JsonWrapper::GetPath((*array)[i]));
        ReferenceResolver::CppToJsonManager::GetInstance().AddReference(res);
        std::cerr << (*array)[i] << std::endl;
    }
    std::cerr << (*array) << std::endl;
    return array;
}

}

