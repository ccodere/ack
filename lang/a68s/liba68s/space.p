88800 #include "rundecs.h"
88810     (*  COPYRIGHT 1983 C.H.LINDSEY, UNIVERSITY OF MANCHESTER  *)
88820  (**)
88830 PROCEDURE GARBAGE (ANOBJECT: OBJECTP); EXTERN ;
88840 PROCEDURE ERRORR(N :INTEGER); EXTERN ;
88850 PROCEDURE CLPASC5(P1,P2 :IPOINT; P3,P4 :INTEGER; P5 :IPOINT; PROC: ASPROC); EXTERN;
88860 PROCEDURE CLRDSTR(PCOV: OBJECTP; VAR CHARS: GETBUFTYPE; TERM (*+01() , TERM1 ()+01*) : TERMSET;
88870                   VAR I: INTEGER; BOOK: FETROOMP; PROC: ASPROC); EXTERN;
88880 PROCEDURE TESTF(RF:OBJECTP;VAR F:OBJECTP); EXTERN;
88890 FUNCTION ENSLINE(RF:OBJECTP;VAR F:OBJECTP):BOOLEAN; EXTERN;
88900 PROCEDURE ERRORSTATE(F:OBJECTP); EXTERN;
88910 (**)
88920 (**)
88930 PROCEDURE SPACE(RF:OBJECTP);
88940   VAR NSTATUS :STATUSSET; F,COV:OBJECTP;
88950       CHARS: GETBUFTYPE; I: INTEGER;
88960     BEGIN FPINC(RF^);
88970     TESTF(RF,F); NSTATUS:=F^.PCOVER^.STATUS;
88980     IF NOT(([OPENED,READMOOD]<=NSTATUS) OR ([OPENED,WRITEMOOD]<=NSTATUS))
88990     THEN ERRORSTATE(F)
89000     ELSE IF [LINEOVERFLOW]<=NSTATUS
89010       THEN IF NOT ENSLINE(RF,F) THEN ERRORR(NOLOGICAL);
89020     (* OPENED,LINEOK,MOODOK *)
89030     COV:=F^.PCOVER;
89040     IF COV^.ASSOC THEN WITH COV^ DO
89050       BEGIN
89060       COFCPOS := COFCPOS+1; CPOSELS := CPOSELS+OFFSETDI;
89070       IF COFCPOS>CHARBOUND THEN STATUS := STATUS+[LINEOVERFLOW];
89080       END
89090     ELSE IF [READMOOD,CHARMOOD]<=F^.PCOVER^.STATUS THEN
89100       BEGIN I := -1; CLRDSTR(COV, CHARS, ALLCHAR (*+01() , ALLCHAR ()+01*) , I, COV^.BOOK, COV^.DOGETS) END
89110     ELSE WITH F^.PCOVER^ DO
89120       CLPASC5(ORD(COV), ORD(F), -1, ORD(' '), ORD(BOOK), DOPUTS);
89130     WITH RF^ DO BEGIN FDEC; IF FTST THEN GARBAGE(RF) END;
89140     END;
89150 (**)
89160 (**)
89170 (*-02()
89180 BEGIN (*OF A68*)
89190 END; (*OF A68*)
89200 ()-02*)
89210 (*+01()
89220 BEGIN (*OF MAIN PROGRAM*)
89230 END (* OF EVERYTHING *).
89240 ()+01*)