
// Copyright (c) 1999-2002 by Digital Mars
// All Rights Reserved
// written by Walter Bright
// www.digitalmars.com
// License for redistribution is by either the Artistic License
// in artistic.txt, or the GNU General Public License in gnu.txt.
// See the included readme.txt for details.

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "root.h"

#include "declaration.h"
#include "mtype.h"
#include "attrib.h"
#include "template.h"

char *Declaration::mangle()
#if __DMC__
    __out(result)
    {
	int len = strlen(result);

	assert(len > 0);
	//printf("mangle: '%s' => '%s'\n", toChars(), result);
	for (int i = 0; i < len; i++)
	{
	    assert(result[i] == '_' || isalnum(result[i]));
	}
    }
    __body
#endif
    {
	OutBuffer buf;
	char *id;
	Dsymbol *s;

	//printf("Declaration::mangle(parent = %p)\n", parent);
	if (!parent || parent->isModule())	// if at global scope
	{
	    // If it's not a D declaration, no mangling
	    switch (linkage)
	    {
		case LINKd:
		    break;

		case LINKc:
		case LINKwindows:
		case LINKpascal:
		case LINKcpp:
		    return ident->toChars();

		default:
		    printf("'%s', linkage = %d\n", toChars(), linkage);
		    assert(0);
	    }
	}

	s = this;
	do
	{
	    buf.prependstring("_");
	    if (s->ident)
		buf.prependstring(s->ident->toChars());
	    s = s->parent;
	} while (s);

	buf.prependstring("_D");
	buf.writestring(type->deco);

	id = buf.toChars();
	buf.data = NULL;
	return id;
    }

char *FuncDeclaration::mangle()
#if __DMC__
    __out(result)
    {
	assert(strlen(result) > 0);
    }
    __body
#endif
    {
	if (isMain())
	    return "_Dmain";

	return Declaration::mangle();
    }

char *StructDeclaration::mangle()
{
    char *id;

    //printf("StructDeclaration::mangle() '%s'\n", toChars());
    if (parent)
    {
	OutBuffer buf;

	//printf("  parent = '%s', kind = '%s'\n", parent->mangle(), parent->kind());
	buf.writestring(parent->mangle());
	buf.writestring("_");
	buf.writestring(ident ? ident->toChars() : toChars());
	id = buf.toChars();
	buf.data = NULL;
    }
    else
	id = ident ? ident->toChars() : toChars();
    return id;
}


char *TypedefDeclaration::mangle()
{
    char *id;

    //printf("TypedefDeclaration::mangle() '%s'\n", toChars());
    if (parent)
    {
	OutBuffer buf;

	//printf("  parent = '%s', kind = '%s'\n", parent->mangle(), parent->kind());
	buf.writestring(parent->mangle());
	buf.writestring("_");
	buf.writestring(ident ? ident->toChars() : toChars());
	id = buf.toChars();
	buf.data = NULL;
    }
    else
	id = ident ? ident->toChars() : toChars();
    return id;
}


char *ClassDeclaration::mangle()
{
    char *id;
#if 1
    id = ident ? ident->toChars() : toChars();
    if (parent)
    {
	/* These are reserved to the compiler, so keep simple
	 * names for them.
	 */
	if (ident == Id::TypeInfo   ||
	    ident == Id::Exception  ||
	    ident == Id::Object     ||
	    ident == Id::ClassInfo  ||
	    ident == Id::ModuleInfo ||
	    memcmp(id, "TypeInfo_", 9) == 0
	   )
	    return id;

	OutBuffer buf;

	//printf("  parent = '%s', kind = '%s'\n", parent->mangle(), parent->kind());
	buf.writestring(parent->mangle());
	buf.writestring("_");
	buf.writestring(id);
	id = buf.toChars();
	buf.data = NULL;
    }
    return id;
#else
    OutBuffer buf;
    Dsymbol *s;

    s = this;
    while (1)
    {
	buf.prependstring(s->ident ? s->ident->toChars() : s->toChars());
	s = s->parent;
	if (!s ||
	    (!s->isTemplateInstance() &&
		!s->isClassDeclaration()))
	    break;
	buf.prependstring("_");
    }

    id = buf.toChars();
    buf.data = NULL;
    return id;
#endif
}


char *TemplateInstance::mangle()
{
    char *id;

    if (parent)
    {
	OutBuffer buf;

	buf.writestring(parent->mangle());
	buf.writestring("_");
	buf.writestring(ident ? ident->toChars() : toChars());
	id = buf.toChars();
	buf.data = NULL;
    }
    else
	id = ident ? ident->toChars() : toChars();
    return id;
}



