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

#include "src/common_cpp/JsonCppProcessors/Common/ReferenceResolverJsonToCpp.hh"

namespace MAUS {
namespace ReferenceResolver {
namespace JsonToCpp {
RefManager& RefManager::GetInstance() {
    if (_instance == NULL) {
        throw(Exceptions::Exception(Exceptions::recoverable,
                    "Attempt to get RefManager before initialisation",
                    "ReferenceResolver::JsonToCpp::RefManager::GetInstance()"));
    }
    return *_instance;
}

bool RefManager::HasInstance() {
    return _instance != NULL;
}

void RefManager::Birth() {
    if (_instance != NULL) {
        throw(Exceptions::Exception(Exceptions::recoverable,
                    "Attempt to birth RefManager when it was already birthed",
                    "ReferenceResolver::JsonToCpp::RefManager::Birth()"));
    }
    _instance = new RefManager();
}

void RefManager::Death() {
    if (_instance == NULL) {
        throw(Exceptions::Exception(Exceptions::recoverable,
                    "Attempt to death RefManager before initialisation",
                    "ReferenceResolver::JsonToCpp::RefManager::Death()"));
    }
    delete _instance;
    _instance = NULL;
}

RefManager* RefManager::_instance(NULL);
}  // namespace JsonToCpp
}  // namespace ReferenceResolver
}  // namespace MAUS
