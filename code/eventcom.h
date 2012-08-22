/* <code/eventcom.h> -- Event Logging Common Definitions
 *
 * Copyright (c) 2001 Ravenbrook Limited.  See end of file for license.
 * $Id$
 *
 * .sources: mps.design.telemetry
 */

#ifndef eventcom_h
#define eventcom_h

#include "mpmtypes.h" /* for Word */
#include "eventdef.h"


/* Types for event fields */

typedef unsigned short EventCode;
typedef unsigned EventKind;

typedef Byte EventStringLen;

typedef struct {
  EventStringLen len;
  char str[EventStringLengthMAX];
} EventStringStruct;

typedef EventStringStruct *EventString;


#define EventNameMAX ((size_t)19)
#define EventCodeMAX ((EventCode)0x0069)


/* Event Kinds --- see <design/telemetry/>
 *
 * All events are classified as being of one event type.
 * They are small enough to be able to be used as shifts within a word.
 */

enum {
  EventKindArena,       /* Per space or arena */
  EventKindPool,        /* Per pool */
  EventKindTrace,       /* Per trace or scan */
  EventKindSeg,         /* Per seg */
  EventKindRef,         /* Per ref or fix */
  EventKindObject,      /* Per alloc or object */
  EventKindUser,        /* User-invoked */
  EventKindLIMIT
};


/* Event type definitions
 *
 * Define various constants for each event type to describe them.
 */

/* Note that enum values can be up to fifteen bits long portably. */
#define EVENT_ENUM(X, name, code, always, kind, count, format) \
  enum { \
    Event##name##Code = code, \
    Event##name##Always = always, \
    Event##name##Kind = EventKind##kind \
  };

EVENT_LIST(EVENT_ENUM, X)


/* Event*Struct -- Event Structures
 *
 * Declare the structures that are used to encode events in the internal event
 * buffers and on the binary telemetry output stream.
 */

/* Event field types -- similar to WriteF* */
typedef void *EventFP;
typedef Addr EventFA;
typedef Word EventFW;
typedef unsigned EventFU;
typedef EventStringStruct EventFS;
typedef double EventFD;

/* Common prefix for all event structures */
typedef struct EventAnyStruct {
  EventCode code;
  Word clock;
} EventAnyStruct;

#define EVENT_STRUCT(X, name, _code, always, kind, count, format) \
  typedef struct Event##name##Struct { \
    EventCode code; \
    Word clock; \
    EVENT_STRUCT_FIELDS_##count format \
  } Event##name##Struct;

#define EVENT_STRUCT_FIELDS_0()
/* The following lines were generated with
   python -c 'for i in range(1, 15): print "#define EVENT_STRUCT_FIELDS_%d(%s) %s" % (i, ", ".join(["p%s" % j for j in range(0, i)]), " ".join("EventF##p%d f%d;" % (j, j) for j in range(0,i)))'
 */
#define EVENT_STRUCT_FIELDS_1(p0) EventF##p0 f0;
#define EVENT_STRUCT_FIELDS_2(p0, p1) EventF##p0 f0; EventF##p1 f1;
#define EVENT_STRUCT_FIELDS_3(p0, p1, p2) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2;
#define EVENT_STRUCT_FIELDS_4(p0, p1, p2, p3) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3;
#define EVENT_STRUCT_FIELDS_5(p0, p1, p2, p3, p4) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3; EventF##p4 f4;
#define EVENT_STRUCT_FIELDS_6(p0, p1, p2, p3, p4, p5) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3; EventF##p4 f4; EventF##p5 f5;
#define EVENT_STRUCT_FIELDS_7(p0, p1, p2, p3, p4, p5, p6) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3; EventF##p4 f4; EventF##p5 f5; EventF##p6 f6;
#define EVENT_STRUCT_FIELDS_8(p0, p1, p2, p3, p4, p5, p6, p7) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3; EventF##p4 f4; EventF##p5 f5; EventF##p6 f6; EventF##p7 f7;
#define EVENT_STRUCT_FIELDS_9(p0, p1, p2, p3, p4, p5, p6, p7, p8) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3; EventF##p4 f4; EventF##p5 f5; EventF##p6 f6; EventF##p7 f7; EventF##p8 f8;
#define EVENT_STRUCT_FIELDS_10(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3; EventF##p4 f4; EventF##p5 f5; EventF##p6 f6; EventF##p7 f7; EventF##p8 f8; EventF##p9 f9;
#define EVENT_STRUCT_FIELDS_11(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3; EventF##p4 f4; EventF##p5 f5; EventF##p6 f6; EventF##p7 f7; EventF##p8 f8; EventF##p9 f9; EventF##p10 f10;
#define EVENT_STRUCT_FIELDS_12(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3; EventF##p4 f4; EventF##p5 f5; EventF##p6 f6; EventF##p7 f7; EventF##p8 f8; EventF##p9 f9; EventF##p10 f10; EventF##p11 f11;
#define EVENT_STRUCT_FIELDS_13(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3; EventF##p4 f4; EventF##p5 f5; EventF##p6 f6; EventF##p7 f7; EventF##p8 f8; EventF##p9 f9; EventF##p10 f10; EventF##p11 f11; EventF##p12 f12;
#define EVENT_STRUCT_FIELDS_14(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) EventF##p0 f0; EventF##p1 f1; EventF##p2 f2; EventF##p3 f3; EventF##p4 f4; EventF##p5 f5; EventF##p6 f6; EventF##p7 f7; EventF##p8 f8; EventF##p9 f9; EventF##p10 f10; EventF##p11 f11; EventF##p12 f12; EventF##p13 f13;

EVENT_LIST(EVENT_STRUCT, X)


/* Event -- event union type
 *
 * Event is the type of a pointer to EventUnion, which is a union of all
 * event structures.  This can be used as the type of any event, decoded
 * by examining event->any.code.
 */

#define EVENT_UNION_MEMBER(X, name, code, always, kind, count, format) \
  Event##name##Struct name;

typedef union EventUnion {
  EventAnyStruct any;
  EVENT_LIST(EVENT_UNION_MEMBER, X)
} EventUnion, *Event;


/* EVENTn -- event emitting macros
 *
 * The macros EVENT0, EVENT1, etc. are used throughout the MPS to emit an
 * event with parameters.  They work by appending the event parameters to
 * an event buffer, which is flushed to the telemetry output stream when
 * full.  EVENT2S is a special case that takes a variable length string.
 */

#ifdef EVENT

#define EVENT2S(name, p0, length, string) \
  BEGIN \
    size_t _string_len = (length); \
    size_t size; \
    size = offsetof(Event##name##Struct, f1.str) + _string_len; \
    EVENT_BEGIN(name, size) \
      _event->f0 = (p0); \
      AVER(_string_len < EventStringLengthMAX); \
      _event->f1.len = (EventStringLen)_string_len; \
      mps_lib_memcpy(_event->f1.str, (string), _string_len); \
    EVENT_END(name, size); \
  END

#define EVENT0(name) EVENT_BEGIN(name, sizeof(EventAnyStruct)) EVENT_END(name, sizeof(EventAnyStruct))
/* The following lines were generated with
   python -c 'for i in range(1,15): print "#define EVENT%d(name, %s) EVENT_BEGIN(name, sizeof(Event##name##Struct)) %s EVENT_END(name, sizeof(Event##name##Struct))" % (i, ", ".join(["p%d" % j for j in range(0, i)]), " ".join(["_event->f%d = (p%d);" % (j, j) for j in range(0, i)]))'
 */
#define EVENT1(name, p0) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT2(name, p0, p1) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT3(name, p0, p1, p2) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT4(name, p0, p1, p2, p3) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT5(name, p0, p1, p2, p3, p4) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); _event->f4 = (p4); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT6(name, p0, p1, p2, p3, p4, p5) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); _event->f4 = (p4); _event->f5 = (p5); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT7(name, p0, p1, p2, p3, p4, p5, p6) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); _event->f4 = (p4); _event->f5 = (p5); _event->f6 = (p6); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT8(name, p0, p1, p2, p3, p4, p5, p6, p7) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); _event->f4 = (p4); _event->f5 = (p5); _event->f6 = (p6); _event->f7 = (p7); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT9(name, p0, p1, p2, p3, p4, p5, p6, p7, p8) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); _event->f4 = (p4); _event->f5 = (p5); _event->f6 = (p6); _event->f7 = (p7); _event->f8 = (p8); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT10(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); _event->f4 = (p4); _event->f5 = (p5); _event->f6 = (p6); _event->f7 = (p7); _event->f8 = (p8); _event->f9 = (p9); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT11(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); _event->f4 = (p4); _event->f5 = (p5); _event->f6 = (p6); _event->f7 = (p7); _event->f8 = (p8); _event->f9 = (p9); _event->f10 = (p10); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT12(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); _event->f4 = (p4); _event->f5 = (p5); _event->f6 = (p6); _event->f7 = (p7); _event->f8 = (p8); _event->f9 = (p9); _event->f10 = (p10); _event->f11 = (p11); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT13(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); _event->f4 = (p4); _event->f5 = (p5); _event->f6 = (p6); _event->f7 = (p7); _event->f8 = (p8); _event->f9 = (p9); _event->f10 = (p10); _event->f11 = (p11); _event->f12 = (p12); EVENT_END(name, sizeof(Event##name##Struct))
#define EVENT14(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) EVENT_BEGIN(name, sizeof(Event##name##Struct)) _event->f0 = (p0); _event->f1 = (p1); _event->f2 = (p2); _event->f3 = (p3); _event->f4 = (p4); _event->f5 = (p5); _event->f6 = (p6); _event->f7 = (p7); _event->f8 = (p8); _event->f9 = (p9); _event->f10 = (p10); _event->f11 = (p11); _event->f12 = (p12); _event->f13 = (p13); EVENT_END(name, sizeof(Event##name##Struct))


#define EVENT0_FIELD_PTR(name, event, i) NULL

/*
  for i in range(1,20): print "#define EVENT%d_FIELD_PTR(name, event, i) ( \\\n  %s\\\n  : NULL)\n" % (i, " \\\n  : ".join(["((i) == %d) ? (void *)&((event)->name.f%d)" % (j, j) for j in range(0, i)]))
 */

#define EVENT1_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0)\
  : NULL)

#define EVENT2_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1)\
  : NULL)

#define EVENT3_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2)\
  : NULL)

#define EVENT4_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3)\
  : NULL)

#define EVENT5_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4)\
  : NULL)

#define EVENT6_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5)\
  : NULL)

#define EVENT7_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6)\
  : NULL)

#define EVENT8_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7)\
  : NULL)

#define EVENT9_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8)\
  : NULL)

#define EVENT10_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8) \
  : ((i) == 9) ? (void *)&((event)->name.f9)\
  : NULL)

#define EVENT11_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8) \
  : ((i) == 9) ? (void *)&((event)->name.f9) \
  : ((i) == 10) ? (void *)&((event)->name.f10)\
  : NULL)

#define EVENT12_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8) \
  : ((i) == 9) ? (void *)&((event)->name.f9) \
  : ((i) == 10) ? (void *)&((event)->name.f10) \
  : ((i) == 11) ? (void *)&((event)->name.f11)\
  : NULL)

#define EVENT13_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8) \
  : ((i) == 9) ? (void *)&((event)->name.f9) \
  : ((i) == 10) ? (void *)&((event)->name.f10) \
  : ((i) == 11) ? (void *)&((event)->name.f11) \
  : ((i) == 12) ? (void *)&((event)->name.f12)\
  : NULL)

#define EVENT14_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8) \
  : ((i) == 9) ? (void *)&((event)->name.f9) \
  : ((i) == 10) ? (void *)&((event)->name.f10) \
  : ((i) == 11) ? (void *)&((event)->name.f11) \
  : ((i) == 12) ? (void *)&((event)->name.f12) \
  : ((i) == 13) ? (void *)&((event)->name.f13)\
  : NULL)

#define EVENT15_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8) \
  : ((i) == 9) ? (void *)&((event)->name.f9) \
  : ((i) == 10) ? (void *)&((event)->name.f10) \
  : ((i) == 11) ? (void *)&((event)->name.f11) \
  : ((i) == 12) ? (void *)&((event)->name.f12) \
  : ((i) == 13) ? (void *)&((event)->name.f13) \
  : ((i) == 14) ? (void *)&((event)->name.f14)\
  : NULL)

#define EVENT16_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8) \
  : ((i) == 9) ? (void *)&((event)->name.f9) \
  : ((i) == 10) ? (void *)&((event)->name.f10) \
  : ((i) == 11) ? (void *)&((event)->name.f11) \
  : ((i) == 12) ? (void *)&((event)->name.f12) \
  : ((i) == 13) ? (void *)&((event)->name.f13) \
  : ((i) == 14) ? (void *)&((event)->name.f14) \
  : ((i) == 15) ? (void *)&((event)->name.f15)\
  : NULL)

#define EVENT17_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8) \
  : ((i) == 9) ? (void *)&((event)->name.f9) \
  : ((i) == 10) ? (void *)&((event)->name.f10) \
  : ((i) == 11) ? (void *)&((event)->name.f11) \
  : ((i) == 12) ? (void *)&((event)->name.f12) \
  : ((i) == 13) ? (void *)&((event)->name.f13) \
  : ((i) == 14) ? (void *)&((event)->name.f14) \
  : ((i) == 15) ? (void *)&((event)->name.f15) \
  : ((i) == 16) ? (void *)&((event)->name.f16)\
  : NULL)

#define EVENT18_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8) \
  : ((i) == 9) ? (void *)&((event)->name.f9) \
  : ((i) == 10) ? (void *)&((event)->name.f10) \
  : ((i) == 11) ? (void *)&((event)->name.f11) \
  : ((i) == 12) ? (void *)&((event)->name.f12) \
  : ((i) == 13) ? (void *)&((event)->name.f13) \
  : ((i) == 14) ? (void *)&((event)->name.f14) \
  : ((i) == 15) ? (void *)&((event)->name.f15) \
  : ((i) == 16) ? (void *)&((event)->name.f16) \
  : ((i) == 17) ? (void *)&((event)->name.f17)\
  : NULL)

#define EVENT19_FIELD_PTR(name, event, i) ( \
  ((i) == 0) ? (void *)&((event)->name.f0) \
  : ((i) == 1) ? (void *)&((event)->name.f1) \
  : ((i) == 2) ? (void *)&((event)->name.f2) \
  : ((i) == 3) ? (void *)&((event)->name.f3) \
  : ((i) == 4) ? (void *)&((event)->name.f4) \
  : ((i) == 5) ? (void *)&((event)->name.f5) \
  : ((i) == 6) ? (void *)&((event)->name.f6) \
  : ((i) == 7) ? (void *)&((event)->name.f7) \
  : ((i) == 8) ? (void *)&((event)->name.f8) \
  : ((i) == 9) ? (void *)&((event)->name.f9) \
  : ((i) == 10) ? (void *)&((event)->name.f10) \
  : ((i) == 11) ? (void *)&((event)->name.f11) \
  : ((i) == 12) ? (void *)&((event)->name.f12) \
  : ((i) == 13) ? (void *)&((event)->name.f13) \
  : ((i) == 14) ? (void *)&((event)->name.f14) \
  : ((i) == 15) ? (void *)&((event)->name.f15) \
  : ((i) == 16) ? (void *)&((event)->name.f16) \
  : ((i) == 17) ? (void *)&((event)->name.f17) \
  : ((i) == 18) ? (void *)&((event)->name.f18)\
  : NULL)

#else /* EVENT not */

#define EVENT0(name) NOOP
/* The following lines were generated with
  python -c 'for i in range(1,15): print "#define EVENT%d(name, %s) NOOP" % (i, ", ".join(["p%d" % j for j in range(0, i)]))'
 */
#define EVENT1(name, p0) NOOP
#define EVENT2(name, p0, p1) NOOP
#define EVENT3(name, p0, p1, p2) NOOP
#define EVENT4(name, p0, p1, p2, p3) NOOP
#define EVENT5(name, p0, p1, p2, p3, p4) NOOP
#define EVENT6(name, p0, p1, p2, p3, p4, p5) NOOP
#define EVENT7(name, p0, p1, p2, p3, p4, p5, p6) NOOP
#define EVENT8(name, p0, p1, p2, p3, p4, p5, p6, p7) NOOP
#define EVENT9(name, p0, p1, p2, p3, p4, p5, p6, p7, p8) NOOP
#define EVENT10(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) NOOP
#define EVENT11(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) NOOP
#define EVENT12(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) NOOP
#define EVENT13(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) NOOP
#define EVENT14(name, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) NOOP

#endif /* EVENT */


#endif /* eventcom_h */


/* C. COPYRIGHT AND LICENSE
 *
 * Copyright (C) 2001-2002 Ravenbrook Limited <http://www.ravenbrook.com/>.
 * All rights reserved.  This is an open source license.  Contact
 * Ravenbrook for commercial licensing options.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * 3. Redistributions in any form must be accompanied by information on how
 * to obtain complete source code for this software and any accompanying
 * software that uses this software.  The source code must either be
 * included in the distribution or be available for no more than the cost
 * of distribution plus a nominal fee, and must be freely redistributable
 * under reasonable conditions.  For an executable file, complete source
 * code means the source code for all modules it contains. It does not
 * include source code for modules or files that typically accompany the
 * major components of the operating system on which the executable file
 * runs.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, OR NON-INFRINGEMENT, ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
