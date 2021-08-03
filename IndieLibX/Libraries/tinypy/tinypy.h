/*
The tinypy License

Copyright (c) 2008 Phil Hassey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef TINYPY_H
#define TINYPY_H
#ifndef TP_H
#define TP_H

#include <setjmp.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#define tp_malloc(x) calloc((x),1)
#define tp_realloc(x,y) realloc(x,y)
#define tp_free(x) free(x)

// #include <gc/gc.h>
// #define tp_malloc(x) GC_MALLOC(x)
// #define tp_realloc(x,y) GC_REALLOC(x,y)
// #define tp_free(x)

enum {
    TP_NONE,TP_NUMBER,TP_STRING,TP_DICT,
    TP_LIST,TP_FNC,TP_DATA,
};

typedef double tp_num;

typedef struct tp_number_ {
    int type;
    tp_num val;
} tp_number_;
typedef struct tp_string_ {
    int type;
    struct _tp_string *info;
    char *val;
    int len;
} tp_string_;
typedef struct tp_list_ {
    int type;
    struct _tp_list *val;
} tp_list_;
typedef struct tp_dict_ {
    int type;
    struct _tp_dict *val;
} tp_dict_;
typedef struct tp_fnc_ {
    int type;
    struct _tp_fnc *val;
    int ftype;
    void *fval;
} tp_fnc_;
typedef struct tp_data_ {
    int type;
    struct _tp_data *info;
    void *val;
    struct tp_meta *meta;
} tp_data_;

typedef union tp_obj {
    int type;
    tp_number_ number;
    struct { int type; int *data; } gci;
    tp_string_ string;
    tp_dict_ dict;
    tp_list_ list;
    tp_fnc_ fnc;
    tp_data_ data;
} tp_obj;

typedef struct _tp_string {
    int gci;
    char s[1];
} _tp_string;
typedef struct _tp_list {
    int gci;
    tp_obj *items;
    int len;
    int alloc;
} _tp_list;
typedef struct tp_item {
    int used;
    int hash;
    tp_obj key;
    tp_obj val;
} tp_item;
typedef struct _tp_dict {
    int gci;
    tp_item *items;
    int len;
    int alloc;
    int cur;
    int mask;
    int used;
} _tp_dict;
typedef struct _tp_fnc {
    int gci;
    tp_obj self;
    tp_obj globals;
} _tp_fnc;


typedef union tp_code {
    unsigned char i;
    struct { unsigned char i,a,b,c; } regs;
    struct { char val[4]; } string;
    struct { float val; } number;
} tp_code;

typedef struct tp_frame_ {
    tp_code *codes;
    tp_code *cur;
    tp_code *jmp;
    tp_obj *regs;
    tp_obj *ret_dest;
    tp_obj fname;
    tp_obj name;
    tp_obj line;
    tp_obj globals;
    int lineno;
    int cregs;
} tp_frame_;

#define TP_GCMAX 4096
#define TP_FRAMES 256
// #define TP_REGS_PER_FRAME 256
#define TP_REGS 16384
typedef struct tp_vm {
    tp_obj builtins;
    tp_obj modules;
    tp_frame_ frames[TP_FRAMES];
    tp_obj _params;
    tp_obj params;
    tp_obj _regs;
    tp_obj *regs;
    tp_obj root;
    jmp_buf buf;
    int jmp;
    tp_obj ex;
    char chars[256][2];
    int cur;
    // gc
    _tp_list *white;
    _tp_list *grey;
    _tp_list *black;
    _tp_dict *strings;
    int steps;
} tp_vm;

#define TP tp_vm *tp
typedef struct tp_meta {
    int type;
    tp_obj (*get)(TP,tp_obj,tp_obj);
    void (*set)(TP,tp_obj,tp_obj,tp_obj);
    void (*free)(TP,tp_obj);
//     tp_obj (*del)(TP,tp_obj,tp_obj);
//     tp_obj (*has)(TP,tp_obj,tp_obj);
//     tp_obj (*len)(TP,tp_obj);
} tp_meta;
typedef struct _tp_data {
    int gci;
    tp_meta meta;
} _tp_data;

// NOTE: these are the few out of namespace items for convenience
#define None ((tp_obj){TP_NONE})
#define True tp_number(1)
#define False tp_number(0)
#define STR(v) ((tp_str(tp,(v))).string.val)

void tp_set(TP,tp_obj,tp_obj,tp_obj);
tp_obj tp_get(TP,tp_obj,tp_obj);
tp_obj tp_len(TP,tp_obj);
tp_obj tp_str(TP,tp_obj);
int tp_cmp(TP,tp_obj,tp_obj);
void _tp_raise(TP,tp_obj);
tp_obj tp_printf(TP,char *fmt,...);
tp_obj tp_track(TP,tp_obj);
void tp_grey(TP,tp_obj);

// __func__ __VA_ARGS__ __FILE__ __LINE__
#define tp_raise(r,fmt,...) { \
    _tp_raise(tp,tp_printf(tp,fmt,__VA_ARGS__)); \
    return r; \
}
#define __params (tp->params)
#define TP_OBJ() (tp_get(tp,__params,None))
inline static tp_obj tp_type(TP,int t,tp_obj v) {
    if (v.type != t) { tp_raise(None,"_tp_type(%d,%s)",t,STR(v)); }
    return v;
}
#define TP_TYPE(t) tp_type(tp,t,TP_OBJ())
#define TP_NUM() (TP_TYPE(TP_NUMBER).number.val)
#define TP_STR() (STR(TP_TYPE(TP_STRING)))
#define TP_DEFAULT(d) (__params.list.val->len?tp_get(tp,__params,None):(d))
#define TP_LOOP(e) \
    int __l = __params.list.val->len; \
    int __i; for (__i=0; __i<__l; __i++) { \
    (e) = _tp_list_get(tp,__params.list.val,__i,"TP_LOOP");
#define TP_END \
    }

inline static int _tp_min(int a, int b) { return (a<b?a:b); }
inline static int _tp_max(int a, int b) { return (a>b?a:b); }
inline static int _tp_sign(tp_num v) { return (v<0?-1:(v>0?1:0)); }
inline static tp_obj tp_number(tp_num v) { return (tp_obj)(tp_number_){TP_NUMBER,v}; }
inline static tp_obj tp_string(char *v) { return (tp_obj)(tp_string_){TP_STRING,0,v,strlen(v)}; }
inline static tp_obj tp_string_n(char *v,int n) {
    return (tp_obj)(tp_string_){TP_STRING,0,v,n};
}

#endif
//
void _tp_list_realloc(_tp_list *self,int len) ;
void _tp_list_set(TP,_tp_list *self,int k, tp_obj v, char *error) ;
void _tp_list_free(_tp_list *self) ;
tp_obj _tp_list_get(TP,_tp_list *self,int k,char *error) ;
void _tp_list_insertx(TP,_tp_list *self, int n, tp_obj v) ;
void _tp_list_appendx(TP,_tp_list *self, tp_obj v) ;
void _tp_list_insert(TP,_tp_list *self, int n, tp_obj v) ;
void _tp_list_append(TP,_tp_list *self, tp_obj v) ;
tp_obj _tp_list_pop(TP,_tp_list *self, int n, char *error) ;
int _tp_list_find(TP,_tp_list *self, tp_obj v) ;
tp_obj tp_index(TP) ;
_tp_list *_tp_list_new(void) ;
tp_obj _tp_list_copy(TP, tp_obj rr) ;
tp_obj tp_append(TP) ;
tp_obj tp_pop(TP) ;
tp_obj tp_insert(TP) ;
tp_obj tp_extend(TP) ;
tp_obj tp_list(TP) ;
tp_obj tp_list_n(TP,int n,tp_obj *argv) ;
int _tp_sort_cmp(tp_obj *a,tp_obj *b) ;
tp_obj tp_sort(TP) ;
int tp_lua_hash(void *v,int l) ;
void _tp_dict_free(_tp_dict *self) ;
// void _tp_dict_reset(_tp_dict *self) ;
int tp_hash(TP,tp_obj v) ;
void _tp_dict_hash_set(TP,_tp_dict *self, int hash, tp_obj k, tp_obj v) ;
void _tp_dict_tp_realloc(TP,_tp_dict *self,int len) ;
int _tp_dict_hash_find(TP,_tp_dict *self, int hash, tp_obj k) ;
int _tp_dict_find(TP,_tp_dict *self,tp_obj k) ;
void _tp_dict_setx(TP,_tp_dict *self,tp_obj k, tp_obj v) ;
void _tp_dict_set(TP,_tp_dict *self,tp_obj k, tp_obj v) ;
tp_obj _tp_dict_get(TP,_tp_dict *self,tp_obj k, char *error) ;
void _tp_dict_del(TP,_tp_dict *self,tp_obj k, char *error) ;
_tp_dict *_tp_dict_new(void) ;
tp_obj _tp_dict_copy(TP,tp_obj rr) ;
int _tp_dict_next(TP,_tp_dict *self) ;
tp_obj tp_merge(TP) ;
tp_obj tp_dict(TP) ;
tp_obj tp_dict_n(TP,int n, tp_obj* argv) ;
tp_obj *tp_ptr(tp_obj o) ;
tp_obj _tp_dcall(TP,tp_obj fnc(TP)) ;
tp_obj _tp_tcall(TP,tp_obj fnc) ;
tp_obj tp_fnc_new(TP,int t, void *v, tp_obj s, tp_obj g) ;
tp_obj tp_def(TP,void *v, tp_obj g) ;
tp_obj tp_fnc(TP,tp_obj v(TP)) ;
tp_obj tp_method(TP,tp_obj self,tp_obj v(TP)) ;
tp_obj tp_data(TP,void *v) ;
tp_obj tp_params(TP) ;
tp_obj tp_params_n(TP,int n, tp_obj argv[]) ;
tp_obj tp_params_v(TP,int n,...) ;
tp_obj tp_string_t(TP, int n) ;
tp_obj tp_printf(TP,char *fmt,...) ;
int _tp_str_index(tp_obj s, tp_obj k) ;
tp_obj tp_join(TP) ;
tp_obj tp_string_slice(TP,tp_obj s, int a, int b) ;
tp_obj tp_split(TP) ;
tp_obj tp_find(TP) ;
tp_obj tp_str_index(TP) ;
tp_obj tp_str2(TP) ;
tp_obj tp_chr(TP) ;
tp_obj tp_ord(TP) ;
tp_obj tp_strip(TP) ;
tp_obj tp_replace(TP) ;
tp_obj tp_print(TP) ;
tp_obj tp_bind(TP) ;
tp_obj tp_min(TP) ;
tp_obj tp_max(TP) ;
tp_obj tp_copy(TP) ;
tp_obj tp_len_(TP) ;
tp_obj tp_assert(TP) ;
tp_obj tp_range(TP) ;
tp_obj tp_system(TP) ;
tp_obj tp_istype(TP) ;
tp_obj tp_float(TP) ;
tp_obj tp_save(TP) ;
tp_obj tp_load(TP) ;
tp_obj tp_fpack(TP) ;
tp_obj tp_abs(TP) ;
tp_obj tp_int(TP) ;
tp_num _roundf(tp_num v) ;
tp_obj tp_round(TP) ;
tp_obj tp_exists(TP) ;
tp_obj tp_mtime(TP) ;
void tp_grey(TP,tp_obj v) ;
void tp_follow(TP,tp_obj v) ;
void tp_reset(TP) ;
void tp_gc_init(TP) ;
void tp_gc_deinit(TP) ;
void tp_delete(TP,tp_obj v) ;
void tp_collect(TP) ;
void _tp_gcinc(TP) ;
void tp_full(TP) ;
void tp_gcinc(TP) ;
tp_obj tp_track(TP,tp_obj v) ;
tp_obj tp_str(TP,tp_obj self) ;
int tp_bool(TP,tp_obj v) ;
tp_obj tp_has(TP,tp_obj self, tp_obj k) ;
void tp_del(TP,tp_obj self, tp_obj k) ;
tp_obj tp_iter(TP,tp_obj self, tp_obj k) ;
tp_obj tp_get(TP,tp_obj self, tp_obj k) ;
int tp_iget(TP,tp_obj *r, tp_obj self, tp_obj k) ;
void tp_set(TP,tp_obj self, tp_obj k, tp_obj v) ;
tp_obj tp_add(TP,tp_obj a, tp_obj b) ;
tp_obj tp_mul(TP,tp_obj a, tp_obj b) ;
tp_obj tp_len(TP,tp_obj self) ;
int tp_cmp(TP,tp_obj a, tp_obj b) ;
tp_vm *_tp_init(void) ;
void tp_deinit(TP) ;
void tp_frame(TP,tp_obj globals,tp_code *codes,tp_obj *ret_dest) ;
void _tp_raise(TP,tp_obj e) ;
void tp_print_stack(TP) ;
void tp_handle(TP) ;
void _tp_call(TP,tp_obj *dest, tp_obj fnc, tp_obj params) ;
void tp_return(TP, tp_obj v) ;
int tp_step(TP) ;
void tp_run(TP,int cur) ;
tp_obj tp_call(TP, char *mod, char *fnc, tp_obj params) ;
tp_obj tp_import(TP,char *fname, char *name, void *codes) ;
tp_obj tp_exec_(TP) ;
tp_obj tp_import_(TP) ;
tp_obj tp_builtins(TP) ;
void tp_args(TP,int argc, char *argv[]) ;
tp_obj tp_main(TP,char *fname, void *code) ;
tp_obj tp_compile(TP, tp_obj text, tp_obj fname) ;
tp_obj tp_exec(TP,tp_obj code, tp_obj globals) ;
tp_obj tp_eval(TP, char *text, tp_obj globals) ;
tp_vm *tp_init(int argc, char *argv[]) ;
void tp_compiler(TP) ;
#endif
