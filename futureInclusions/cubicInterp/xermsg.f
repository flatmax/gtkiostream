      SUBROUTINE XERMSG(LIBNAME, SUBNAME, ERRMSG, ERRCODE, RETCODE)
      CHARACTER*(*) LIBNAME, SUBNAME, ERRMSG
      INTEGER       ERRCODE, RETCODE
C
C     only print fatal errors
C
      IF (RETCODE .GT. 0) THEN
          WRITE(*, FMT='(1X, A, '':'', A, '':'', 1X, A)') LIBNAME,
     +          SUBNAME, ERRMSG
          WRITE(*, FMT='(1X, ''ERRORCODE = '', I)') ERRCODE
          WRITE(*, FMT='(1X, ''RETCODE   = '', I)') RETCODE
      ENDIF
C
      END

