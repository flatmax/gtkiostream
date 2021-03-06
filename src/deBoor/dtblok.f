      subroutine dtblok ( bloks, integs, nbloks, ipivot, iflag,
     &                    detsgn, detlog )

c*********************************************************************72
c
cc DTBLOK gets the determinant of an almost block diagonal matrix.
c
c  computes the determinant of an almost block diagonal matrix whose
c  plu factorization has been obtained previously in fcblok.
c  *** the logarithm of the determinant is computed instead of the
c  determinant itself to avoid the danger of overflow or underflow
c  inherent in this calculation.
c
c parameters
c    bloks, integs, nbloks, ipivot, iflag  are as on return from fcblok.
c            in particular, iflag = (-1)**(number of interchanges dur-
c            ing factorization) if successful, otherwise iflag = 0.
c    detsgn  on output, contains the sign of the determinant.
c    detlog  on output, contains the natural logarithm of the determi-
c            nant if determinant is not zero. otherwise contains 0.
c
      implicit none

      integer nbloks

      integer index,integs(3,nbloks),ipivot(1),iflag, i,indexp,ip,
     &  k,last,nrow
      double precision bloks(1),detsgn,detlog
c
      detsgn = iflag
      detlog = 0.0D+00
      if (iflag .eq. 0)                 return
      index = 0
      indexp = 0
      do 2 i=1,nbloks
         nrow = integs(1,i)
         last = integs(3,i)
         do 1 k=1,last
            ip = index + nrow*(k-1) + ipivot(indexp+k)
            detlog = detlog + dlog(dabs(bloks(ip)))
    1       detsgn = detsgn*dsign(1.0D+00,bloks(ip))
         index = nrow*integs(2,i) + index
    2    indexp = indexp + nrow
                                        return
      end
