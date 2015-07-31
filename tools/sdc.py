#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#   This file is part of the KDE project
#   Copyright (C) 2010-2015 Jarosław Staniek <staniek@kde.org>
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public
#   License as published by the Free Software Foundation; either
#   version 2 of the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Library General Public License for more details.
#
#   You should have received a copy of the GNU Library General Public License
#   along with this program; see the file COPYING.  If not, write to
#   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#   Boston, MA 02110-1301, USA.

#
# Shared Data Compiler
#

import os, sys, shlex

version = '0.2'
line = ''

def usage():
    print '''Usage: %s [INPUT] [OUTPUT]
Shared Data Compiler version %s
''' % (sys.argv[0], version)

def syntax_error(msg):
    print "Syntax error in %s: %s" % (in_fname, msg)
    sys.exit(1)

if len(sys.argv) < 3:
    usage()
    sys.exit(0)

# --- open ---
in_fname = sys.argv[1]
out_fname = sys.argv[2]

try:
    infile = open(in_fname, "rb") # binary mode needed for Windows
    outfile = open(out_fname, "w")
except Exception, inst:
    print inst
    sys.exit(1)

outfile_sdc = None

# --- utils ---
def param(lst, name):
    for item in lst:
        s = item.split('=')
        if len(s) > 1 and s[0] == name:
            return s[1]
    return ''

def param_exists(lst, name):
    try:
        if lst.index(name) >= 0:
            return True
    except ValueError:
            pass
    return False


# --- process ---
shared_class_name = ''
shared_class_options = {}
generated_code_inserted = False
shared_class_inserted = False
data_class_ctor = ''
data_class_copy_ctor = ''
data_class_members = ''
members_list = []
data_accesors = ''
protected_data_accesors = ''
main_ctor = ''
member = {}
toMap_impl = ''
fromMap_impl = ''

def get_file(fname):
    if fname.rfind(os.sep) == -1:
        return fname
    return fname[fname.rfind(os.sep)+1:]

def open_sdc():
    global outfile_sdc, shared_class_options
    if not outfile_sdc:
        sdc_fname = out_fname.replace('.h', '_sdc.cpp')
        try:
            outfile_sdc = open(sdc_fname, "w")
        except Exception, inst:
            print inst
            sys.exit(1)
    outfile_sdc.write(warningHeader())
    outfile_sdc.write("""#include "%s"
#include <QVariant>

""" % get_file(out_fname))

    if shared_class_options['namespace']:
        outfile_sdc.write("""using namespace %s;

""" % shared_class_options['namespace'])

"""
    Inserts generated fromMap(), toMap() code into the output.
    Declarations are inserted into the header, definitions into extra *_sdc.cpp file
"""
def insert_fromMap_toMap_methods():
    global outfile, shared_class_name, toMap_impl, fromMap_impl
    outfile.write("""    /*! @return map with saved attributes of the %s object.

     @see %s(const QMap<QString, QString>&, bool *).
    */
    QMap<QString, QString> toMap() const {
        return d->toMap();
    }

""" % (shared_class_name, shared_class_name))
    open_sdc()
    outfile_sdc.write("""%s::Data::Data(const QMap<QString, QString>& map, bool *ok)
{
%s
    if (ok)
        *ok = true;
}

QMap<QString, QString> %s::Data::toMap() const
{
    QMap<QString, QString> map;
%s
    return map;
}
""" % (shared_class_name, fromMap_impl, shared_class_name, toMap_impl))
    outfile_sdc.close()

"""
    Inserts generated operator==() code for shared class into the output.
"""
def insert_operator_eq():
    global outfile, shared_class_name, superclass
    outfile.write("""    //! @return true if this object is equal to @a other; otherwise returns false.
    bool operator==(const %s& other) const {
        return *%s == *other.%s;
    }

""" % (shared_class_name, 'data()' if superclass else 'd', 'data()' if superclass else 'd'))

"""
    Inserts generated clone() method (makes sense for explicitly shared class).
"""
def insert_clone():
    global outfile, shared_class_name
    outfile.write("""    //! Clones the object with all attributes; the copy isn't shared with the original.
    %s clone() {
        return %s(d->clone());
    }
""" % (shared_class_name, shared_class_name))

"""
    Inserts generated Data::operator==() code into the output.
"""
def insert_data_operator_eq():
    global outfile, members_list
    outfile.write("""        bool operator==(const Data& other) const {
""")
    outfile.write("            return ")
    first = True;
    space = ""
    for member in members_list:
        outfile.write("""%s%s == other.%s""" % (space, member, member))
        if first:
            first = False
            space = """
                && """
    outfile.write(""";
        }

""")

"""
    Inserts generated code into the output.
"""
def insert_generated_code(context):
    global infile, outfile, generated_code_inserted, data_class_ctor, data_class_copy_ctor, superclass
    global data_class_members, data_accesors, protected_data_accesors, main_ctor, shared_class_name, shared_class_options
    global prev_line
    if generated_code_inserted:
        return;
    #print "--------insert_generated_code--------"
    #outfile.write('//CONTEXT:' + str(context) + '\n')
    #outfile.write('//data_class_ctor>\n')
    outfile.write(data_class_ctor)
    #outfile.write('//<data_class_ctor\n')
    outfile.write("""        {
        }
""")
    outfile.write(data_class_copy_ctor)
    outfile.write("""        {
        }

""")
    outfile.write("""        virtual ~Data() {}

        virtual %sData* clone() const { return new Data(*this); }

""" % ((superclass + '::') if superclass else ''))

    if shared_class_options['with_from_to_map']:
        outfile.write("""        /*! Constructor for Data object, takes attributes saved to map @a map.
        If @a ok is not 0, *ok is set to true on success and to false on failure. @see toMap(). */
        Data(const QMap<QString, QString>& map, bool *ok);

        QMap<QString, QString> toMap() const;

""")
    if shared_class_options['operator==']:
        insert_data_operator_eq()

    outfile.write(data_class_members)
    outfile.write(main_ctor)
    outfile.write(data_accesors)
    outfile.write("\n")
    if shared_class_options['with_from_to_map']:
        insert_fromMap_toMap_methods()
    if shared_class_options['operator==']:
        insert_operator_eq()
    if shared_class_options['explicit'] and not superclass:
        insert_clone()
    if protected_data_accesors:
        outfile.write("protected:")
        outfile.write(protected_data_accesors)
        outfile.write("\npublic:")
    outfile.write("\n")
    generated_code_inserted = True


"""
    Reads documentation for single section (setter or getter) and returns it.
    Leaves the file pointer before */ or another @getter/@setter mark.
"""
def read_getter_or_setter_doc():
    global prev_line, line
    result = ''
    while True:
        prev_pos = infile.tell()
        prev_line = line
        line = infile.readline()
        if not line:
            break
        elif line.find('*/') != -1 or line.find('@getter') != -1 or line.find('@setter') != -1:
            #print "seek prev from " + line
            infile.seek(prev_pos)
            break
        else:
            result += line
    return result

def process_docs(comment):
    global prev_line, line
    result = {}
    while True:
        prev_line = line
        line = infile.readline()
        #print "process_docs: " + line
        if not line:
            break
        elif line.find('*/') != -1:
            if result == {}:
                insert_generated_code(1)
                outfile.write(line)
            break
        elif line.find('@getter') != -1:
            result['getter'] = read_getter_or_setter_doc()
        elif line.find('@setter') != -1:
            result['setter'] = read_getter_or_setter_doc()
        else:
            insert_generated_code(2)
            outfile.write(comment)
            outfile.write(line)
    if result == {}:
        result = None
    #print "process_docs result: " + str(result)
    return result

def try_read_member_docs(comment):
    global prev_line, line
    prev_pos = infile.tell()
    result = comment
    while True:
        prev_line = line
        line = infile.readline()
        if not line or line.find('@getter') != -1 or line.find('@setter') != -1:
            infile.seek(prev_pos)
            return None
        elif line.find('*/') != -1:
            return result
        else:
            result += line
    return None

""" makes setter out of name or returns forceSetter if specified """
def makeSetter(name, forceSetter):
    if forceSetter:
        return forceSetter
    return 'set' + name[0].upper() + name[1:]

def update_data_accesors():
    global data_accesors, protected_data_accesors, member
    if not member['no_getter']:
        if member.has_key('getter_docs'):
            val = '\n    /*!\n' + member['getter_docs'] + '    */'
            if member['access'] == 'public':
                data_accesors += val
            else: # protected
                protected_data_accesors += val
        getter = member['getter']
        if not getter:
            getter = member['name']
        invokable = 'Q_INVOKABLE ' if member['invokable'] else ''
        if member['custom_getter']:
            val = """
    %s%s %s() const;
""" % (invokable, member['type'], getter)
        else:
            val = """
    %s%s %s() const {
        return %s->%s;
    }
""" % (invokable, member['type'], getter, 'data()' if superclass else 'd', member['name'])
        if member['access'] == 'public':
            data_accesors += val
        else: # protected
            protected_data_accesors += val
    if not member['no_setter']:
        if member.has_key('setter_docs'):
            val = '\n    /*!\n' + member['setter_docs'] + '    */'
            if member['access'] == 'public':
                data_accesors += val
            else: # protected
                protected_data_accesors += val
        # heuristics to check if the const & should be used:
        arg_type = member['type']
        if arg_type.lower() != arg_type and not member['simple_type']:
            arg_type = 'const %s&' % arg_type
        setter = makeSetter(member['name'], member['setter'])
        default_setter = (' = ' + member['default_setter']) if member['default_setter'] else ''
        invokable = 'Q_INVOKABLE ' if member['invokable'] else ''
        if member['custom_setter']:
            val = """
    %svoid %s(%s %s%s);
""" % (invokable, setter, arg_type, member['name'], default_setter)
        else:
            val = """
    %svoid %s(%s %s%s) {
        %s->%s = %s;
    }
""" % (invokable, setter, arg_type, member['name'], default_setter, 'data()' if superclass else 'd', member['name'], member['name'])
        if member['access'] == 'public':
            data_accesors += val
        else: # protected
            protected_data_accesors += val

def data_member_found(lst):
    if len(lst) > 1 and lst[0] == 'data_member':
        return True
    if len(lst) > 2 and lst[0] == 'protected' and lst[1] == 'data_member':
        return True
    if len(lst) > 1 and lst[0] == 'data_method':
        return True
    return False

# sets shared_class_options[option_name] to proper value; returns lst with removed element option_name if exists
def get_shared_class_option(lst, option_name):
    global shared_class_options
    shared_class_options[option_name] = param_exists(lst, option_name)
    if shared_class_options[option_name]:
        lst.remove(option_name)
    return lst

""" like get_shared_class_option() but also gets value (not just checks for existence) """
def get_shared_class_option_with_value(lst, option_name):
    global shared_class_options
    for item in lst:
        s = item.split('=')
        if len(s) > 1 and s[0] == option_name:
            lst.remove(item)
            shared_class_options[option_name] = s[1]
            return lst
    shared_class_options[option_name] = False
    return lst

def warningHeader():
    return """/****************************************************************************
** Shared Class code from reading file '%s'
**
** Created
**      by: The Shared Data Compiler version %s
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

""" % (get_file(in_fname), version)

""" generates conversion code to string from many types, used by Data::toMap()
    @todo more types """
def generate_toString_conversion(name, _type):
    if _type == 'QString' or _type == 'QByteArray':
        return name
    elif _type == 'bool':
        return 'QString::number((int)%s)' % name # 0 or 1
    return 'QVariant(%s).toString()' % name

""" generates conversion code from string to many types, used by Data(QMap<..>)
    @todo more types """
def generate_fromString_conversion(name, _type):
    s = 'map[QLatin1String(\"%s\")]' % name
    if _type == 'bool': # 0 or 1
        return """%s = %s.toInt(ok) == 1;
    if (ok && !(*ok))
        return;
""" % (name, s)
    elif _type == 'int':
        return """%s = %s.toInt(ok);
    if (ok && !(*ok))
        return;
""" % (name, s)
    else: # QString...
        return "%s = %s;" % (name, s)

# returns position (line number) for #include <QSharedData> or -1 if #include <QSharedData> isn't needed
def get_pos_for_QSharedData_h():
    global infile, prev_line, line
    prev_pos = infile.tell()
    line_number = -1
    infile.seek(0)
    # find last #include
    last_include = -1
    while True:
        prev_line = line
        line = infile.readline().lower()
        if not line:
            break
        line_number += 1
        if line.find('#include ') != -1:
            if line.find('qshareddata') != -1:
                last_include = -1
                break
            else:
                last_include = line_number + 1
    infile.seek(prev_pos)
    return last_include

# replaces "Foo<ABC<DEF>>" with "Foo< ABC< DEF > >" to avoid build errors
def fix_templates(s):
    if s.count('<') < 2:
        return s
    result=''
    for c in s:
        if c == '>':
            result += ' '
        result += c
        if c == '<':
            result += ' '
    return result

def other_comment(line):
    ln = line.strip(' ')
    return ln.startswith('/**') \
      or ln.startswith('/*!') \
      or ln .startswith('//!') \
      or ln.startswith('///')

""" @return name of shared class, possibly with full 'inheriting' section name and superclass """
def get_shared_class_name_and_inheritance(lst):
    if lst[-2] == 'public' and lst[-3] == ':': # <name> : public <inherited>
        return (lst[-4], ' ' + lst[-3] + ' ' + lst[-2] + ' ' + lst[-1], lst[-1])
    else:
        return (lst[-1], '', '') # <name>

def process():
    global infile, outfile, generated_code_inserted, data_class_ctor, data_class_copy_ctor
    global shared_class_name, superclass, shared_class_options, shared_class_inserted, data_class_members
    global members_list, data_accesors, member, main_ctor, toMap_impl, fromMap_impl
    global prev_line, line
    outfile.write(warningHeader())

    member = {}
    after_member = False

    data_class_ctor = ''
    data_class_copy_ctor = ''
    data_class_ctor_changed = False
    data_class_copy_ctor_changed = False
    position_for_include_QSharedData_h = get_pos_for_QSharedData_h()

    line_number = -1 # used for writing #include <QSharedData>
    while True:
        prev_line = line
        line = infile.readline()
        line_number += 1
        if not line:
            break
#        print line,
        lst = line.split()
#        print lst
        if line_number == position_for_include_QSharedData_h:
            outfile.write("""#include <QSharedData>
""")
            position_for_include_QSharedData_h = -1
        if line.startswith('class'):
            shared_class_inserted = False # required because otherwise QSharedDataPointer<Data> d will be added to all classes
            outfile.write(line)
        elif line.startswith('shared class'):
            # re-init variables, needed if there are more than one shared class per file
            shared_class_inserted = False
            generated_code_inserted = False
            shared_class_options = {}
            data_class_ctor = ''
            data_class_copy_ctor = ''
            data_class_members = ''
            members_list = []
            data_accesors = ''
            protected_data_accesors = ''
            toMap_impl = ''
            fromMap_impl = ''
            data_class_ctor_changed = False
            data_class_copy_ctor_changed = False
            lst = shlex.split(line) # better than default split()
            # syntax: shared class export=<EXPORT> inherits=<INHERITANCE> <NAME>
            # INHERITANCE is e.g. inherits="public Foo" - use quotes
            # output: class <EXPORT> <NAME>
            export = param(lst, 'export')
            inherits = param(lst, 'inherits')
            lst = get_shared_class_option(lst, 'explicit')
            lst = get_shared_class_option(lst, 'operator==')
            lst = get_shared_class_option(lst, 'with_from_to_map')
            lst = get_shared_class_option(lst, 'virtual_dtor')
            lst = get_shared_class_option_with_value(lst, 'namespace')
            (shared_class_name, shared_class_inheritance, superclass) = get_shared_class_name_and_inheritance(lst)
            if superclass:
                shared_class_options['virtual_dtor'] = True # inheritance implies this
            main_ctor = """    };

    %s()
     : %s(new Data)
    {
    }

    %s(const %s& other)
     : %s
    {
    }
""" % (shared_class_name, superclass if superclass else 'd', shared_class_name, shared_class_name, (superclass + '(other)') if superclass else 'd(other.d)')
            if superclass:
                main_ctor += """
    %s(const %s& other)
     : %s(other)
    {
        if (!data()) { // '@a 'other' does not store suitable data, create a new one and copy what we have
            d = new Data(*d.data());
        }
    }
""" % (shared_class_name, superclass, superclass)

            if shared_class_options['with_from_to_map']:
                main_ctor += """
    /*! Constructor for %s object, takes attributes saved to map @a map.
         If @a ok is not 0, sets *ok to true on success and to false on failure. @see toMap(). */
    %s(const QMap<QString, QString>& map, bool *ok)
     : d(new Data(map, ok))
    {
    }
""" % (shared_class_name, shared_class_name)
            main_ctor += """
    %s~%s();
""" % (('virtual ' if shared_class_options['virtual_dtor'] else ''), shared_class_name)
            if export:
                name = export + ' ' + shared_class_name + shared_class_inheritance
            if inherits:
                inherits = ' : ' + inherits
            if shared_class_options['explicit']:
                outfile.write("""/*! @note objects of this class are explicitly shared, what means they behave like regular
          C++ pointers, except that by doing reference counting and not deleting the shared
          data object until the reference count is 0, they avoid the dangling pointer problem.
          See <a href="http://doc.qt.io/qt-5/qexplicitlyshareddatapointer.html#details">Qt documentation</a>.
 */
""")
            else:
                outfile.write("""/*! @note objects of this class are implicitly shared, what means they have value semantics
          by offering copy-on-write behaviour to maximize resource usage and minimize copying.
          Only a pointer to the data is passed around. See <a href="http://doc.qt.io/qt-5/qshareddatapointer.html#details">Qt documentation</a>.
 */
""")

            outfile.write("class %s%s\n" % (name, inherits))
            while True:
                prev_line = line
                line = infile.readline() # output everything until 'public:'
                outfile.write(line)
                if line.strip().startswith('public:'):
                    break
            shared_class_inserted = True
        elif len(lst) >= 2 and lst[0] == '#if' and lst[1] == '0':
            insert_generated_code(3)
            outfile.write(line)
            while True:
                prev_line = line
                line = infile.readline()
                lst = line.split()
                if not line:
                    break
                elif len(lst) >= 1 and lst[0] == '#endif':
                    outfile.write(line)
                    break
                outfile.write(line)
        elif len(lst) == 1 and (lst[0] == '/**' or lst[0] == '/*!'):
            comment = line

            member_docs = try_read_member_docs(comment)
            #print "member_docs:" + str(member_docs)

            docs = None
            if member_docs:
                member = {}
                member['docs'] = '\n    ' + member_docs.replace('\n', '\n    ') + '    */';
            else:
                docs = process_docs(comment)
            if docs:
                #print "DOCS:" + str(docs)
                member = {}
                if docs.has_key('getter'):
                    member['getter_docs'] = docs['getter']
                if docs.has_key('setter'):
                    member['setter_docs'] = docs['setter']
            elif not member_docs:
                insert_generated_code(4)
                outfile.write(comment)
        elif len(lst) >= 2 and lst[0] == 'enum':
            # skip enums: until '};' found
            outfile.write(line)
            while True:
                prev_line = line
                line = infile.readline()
                lst = line.split()
                if len(lst) > 0 and lst[0] == '};':
                    outfile.write(line)
                    break
                outfile.write(line)
        elif data_member_found(lst):
            if lst[-1].endswith(';'):
                lst[-1] = lst[-1][:-1]
            #print lst
            """ syntax: data_member TYPE NAME [default=DEFAULT_VALUE]
                                              [no_getter] [getter=CUSTOM_GETTER_NAME]
                                              [custom]
                                              [custom_getter]
                                              [default_setter=DEFAULT_SETTER'S_PARAM]
                                              [no_setter] [setter=CUSTOM_SETTER_NAME]
                                              [custom_setter]
                                              [mutable] [simple_type]
                                              [invokable] [explicit]
            output: getter, setter methods, data memeber """
            if lst[0] == 'data_method':
                #if member.has_key('docs'):
                #    data_class_members += member['docs'] + '\n'
                #    del member['docs']
                data_class_members += "        %s;\n" % (' '.join(lst[1:]))
                continue
            elif lst[0] == 'protected':
                member['access'] = 'protected'
                lst = lst[1:]
            else:
                member['access'] = 'public'
            member['type'] = fix_templates(lst[1])
            member['name'] = lst[2]
            members_list.append(member['name']);
            member['default'] = param(lst, 'default')
            member['default_setter'] = param(lst, 'default_setter')
            member['no_getter'] = param_exists(lst, 'no_getter')
            member['getter'] = param(lst, 'getter')
            member['no_setter'] = param_exists(lst, 'no_setter')
            member['setter'] = param(lst, 'setter')
            member['custom'] = param_exists(lst, 'custom')
            member['custom_getter'] = param_exists(lst, 'custom_getter') or member['custom']
            member['custom_setter'] = param_exists(lst, 'custom_setter') or member['custom']
            member['mutable'] = param_exists(lst, 'mutable')
            member['simple_type'] = param_exists(lst, 'simple_type')
            member['invokable'] = param_exists(lst, 'invokable')
            #print member
            if not data_class_ctor_changed:
                data_class_ctor = """    //! @internal data class used to implement %s shared class %s.
    //! Provides thread-safe reference counting.
    class Data : public %s
    {
    public:
        Data()
""" % ('explicitly' if shared_class_options['explicit'] else 'implicitly', shared_class_name, (superclass + '::Data') if superclass else 'QSharedData')
            if not data_class_copy_ctor_changed:
                data_class_copy_ctor = ''
                if superclass:
                    data_class_copy_ctor += """
        Data(const %s::Data& other)
         : %s::Data(other)
        {
        }
""" % (superclass, superclass)
                data_class_copy_ctor += """
        Data(const Data& other)
         : %s(other)
""" % ((superclass + '::Data') if superclass else 'QSharedData')
                data_class_copy_ctor_changed = True
            if member['default']:
                data_class_ctor += '        '
                if data_class_ctor_changed:
                    data_class_ctor += ', '
                else:
                    data_class_ctor += ': '
                    data_class_ctor_changed = True
                data_class_ctor += member['name'] + '(' + member['default'] + ')\n'
#            print data_class_ctor
            data_class_copy_ctor += '         , %s(other.%s)\n' % (member['name'], member['name'])
            if member.has_key('docs'):
                data_class_members += member['docs']

            isInternalMember = member['no_getter'] and member['no_setter']
            mutable = 'mutable ' if member['mutable'] else ''
            data_class_members += "        %s%s %s;" % (mutable, member['type'], member['name'])
            # add doc for shared data member
            if isInternalMember:
                data_class_members += " //!< @internal"
            else:
                data_class_members += " //!< @see "
            if not member['no_getter']:
                getter = member['getter']
                if not getter:
                    getter = member['name']
                data_class_members += "%s::%s()" % (shared_class_name, getter)
            if not member['no_setter']:
                if not member['no_getter']:
                    data_class_members += ", "
                setter = makeSetter(member['name'], member['setter'])
                data_class_members += "%s::%s()" % (shared_class_name, setter)
            data_class_members += "\n"

            if shared_class_options['with_from_to_map']:
                toMap_impl += '    map[QLatin1String(\"%s\")] = %s;\n' % (member['name'], generate_toString_conversion(member['name'], member['type']))
                fromMap_impl += '    %s\n' % generate_fromString_conversion(member['name'], member['type'])
            update_data_accesors()
            member = {}
            if len(prev_line.split()) == 0: # remove current line because it's not going to the output now; this helps to remove duplicated empty lines
                line = ''
            after_member = True
        elif len(lst) > 0 and lst[0] == '};' and line[:2] == '};' and shared_class_inserted:
            insert_generated_code(5)
#            outfile.write('\nprivate:\n');
            outfile.write('\nprotected:\n');
            if shared_class_options['explicit']:
                if superclass:
                    outfile.write("""    virtual const Data* data() const { return dynamic_cast<const Data*>(%s::d.constData()); }
    virtual Data* data() { return dynamic_cast<Data*>(%s::d.data()); }
""" % (superclass, superclass))
                else:
                    outfile.write("""    %s(Data *data)
     : d(data)
    {
    }

    %s(QExplicitlySharedDataPointer<%s::Data> &data)
     : d(data)
    {
    }

    QExplicitlySharedDataPointer<Data> d;
""" % (shared_class_name, shared_class_name, shared_class_name))
            else:
                outfile.write('    QSharedDataPointer<Data> d;\n');
            outfile.write(line)

            if shared_class_options['explicit']:
                outfile.write("""
template<>
%s %s::Data *QSharedDataPointer<%s::Data>::clone();
""" % (export, shared_class_name, shared_class_name))
                open_sdc()
                outfile_sdc.write("""template<>
%s %s::Data *QSharedDataPointer<%s::Data>::clone()
{
    return d->clone();
}
""" % (export, shared_class_name, shared_class_name))

        else:
            #outfile.write('____ELSE____\n');
            if False and other_comment(line):
                prev_pos = infile.tell()
                prev_line_number = line_number
                ln = line[:-1].strip(' ')
                result = ''
                print "'" + ln + "'"
                if ln.startswith('/**') or ln.startswith('/*!'):
                    while True:
                        result += line
                        if not line or ln.endswith('*/'):
                            result = result[:-1]
                            break
                        prev_line = line
                        line = infile.readline()
                        line_number += 1
                        ln = line[:-1].strip(' ')
                    print "!!"
                    print result
                    print "!!"
                if result:
                    member['docs'] = result
                infile.seek(prev_pos)
                prev_line = line
                line = infile.readline()
                lst = line.split()
                line_number = prev_line_number

            if not lst:
                if len(prev_line.split()) > 0:
                    #outfile.write('['+prev_line + ']prev_line[' + line +']')
                    outfile.write(line)
            elif not after_member or len(lst) > 0:
                if shared_class_inserted:
                    insert_generated_code(6)
                outfile.write(line)
            elif generated_code_inserted and len(lst) == 0:
                #outfile.write('ELSE>>>' + line)
                outfile.write(line)
#            else:
#                outfile.write(line)

process()

# --- close ---
infile.close()
outfile.close()
