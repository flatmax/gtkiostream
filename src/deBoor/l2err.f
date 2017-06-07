      subroutine l2err ( prfun , ftau , error )

c*********************************************************************72
c
cc L2ERR computes the errors of an L2 approximation.
c
c  from  * a practical guide to splines *  by c. de boor    
c  this routine is to be called in the main program  l 2 m a i n .
calls subprogram  ppvalu(interv)
c  this subroutine computes various errors of the current l2-approxi-
c  mation , whose pp-repr. is contained in common block  approx  ,
c  to the given data contained in common block  data . it prints out
c  the average error  e r r l 1 , the l2-error  e r r l 2,  and the
c  maximum error  e r r m a x .
c
c******  i n p u t  ******
c  prfun  a logical value.  if prfun = TRUE, the routine prints out
c          the value of the approximation as well as its error at
c          every data point.
c
c******  o u t p u t  ******
c  ftau(1), ..., ftau(ntau),  with  ftau(i)  the approximation  f at
c          tau(i), all i.
c  error(1), ..., error(ntau),  with  error(i) = scale*(g - f)
c          at tau(i), all i. here,  s c a l e  equals  1. in case
c          prfun .ne. 'ON' , or the abs.error is greater than 100 some-
c          where. otherwise, s c a l e  is such that the maximum of
c          abs(error))  over all  i  lies between  10  and  100. this
c          makes the printed output more illustrative.
c
      implicit none

      logical prfun
      integer ie,k,l,ll,lpkmax,ltkmax,ntau,ntmax,on
      double precision ftau(ntau),error(ntau),
     &  break,coef,err,errmax,errl1,errl2,gtau,ppvalu,
     &  scale,tau,totalw,weight

      parameter (lpkmax=100,ntmax=200,ltkmax=2000)

      common / i4data / ntau
      common / r8data / tau(ntmax),gtau(ntmax),weight(ntmax),totalw
      common /approx/ break(lpkmax),coef(ltkmax),l,k

      errl1 = 0.0D+00
      errl2 = 0.0D+00
      errmax = 0.0D+00

      do ll=1,ntau
         ftau(ll) = ppvalu (break, coef, l, k, tau(ll), 0 )
         error(ll) = gtau(ll) - ftau(ll)
         err = dabs(error(ll))
         if (errmax .lt. err)   errmax = err
         errl1 = errl1 + err*weight(ll)
         errl2 = errl2 + err**2*weight(ll)
      end do

      errl1 = errl1/totalw
      errl2 = dsqrt(errl2/totalw)
      print 615,errl2,errl1,errmax
  615 format(///' least square error =',e20.6/
     &          ' average error      =',e20.6/
     &          ' maximum error      =',e20.6//)
c
c  Scale error curve and print.
c
      if ( prfun ) then

        ie = 0
        scale = 1.0D+00
        if (errmax .ge. 10.0D+00)              go to 18

        do ie=1,9
           scale = scale*10.0D+00
           if (errmax*scale .ge. 10.0D+00)     go to 18
        end do

   18   do 19 ll=1,ntau
   19    error(ll) = error(ll)*scale

        print 620,ie,(ll,tau(ll),ftau(ll),error(ll),ll=1,ntau)
  620 format(///14x,'approximation and scaled error curve'/7x,
     &'data point',7x,'approximation',3x,'deviation x 10**',i1/
     &(i4,f16.8,f16.8,f17.6))

      end if

      return
      end
