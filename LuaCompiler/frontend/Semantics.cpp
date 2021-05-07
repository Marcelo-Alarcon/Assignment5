#include <vector>
#include <set>

#include "antlr4-runtime.h"

#include "intermediate/symtab/SymtabStack.h"
#include "intermediate/symtab/Symtab.h"
#include "intermediate/symtab/SymtabEntry.h"
#include "intermediate/symtab/Predefined.h"
#include "intermediate/type/Typespec.h"
#include "intermediate/type/TypeChecker.h"
#include "intermediate/util/CrossReferencer.h"
#include "SemanticErrorHandler.h"
#include "Semantics.h"

namespace intermediate { namespace symtab {
    int Symtab::unnamedIndex = 0;
}}

namespace frontend {

using namespace std;
using namespace intermediate::symtab;
using namespace intermediate::type;
using namespace intermediate::util;

} // namespace frontend
