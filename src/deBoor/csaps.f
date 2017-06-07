      subroutine csaps ( x, y, dy, npoint, s, smY )
c chapter xiv, example 1. cubic smoothing spline
c  from  * a practical guide to splines *  by c. de boor    
calls  bsplpp(bsplvb), ppvalu(interv), smooth(setupq,chol1d)
c     values from a cubic b-spline are rounded to  n d i g i t  places
c  after the decimal point, then smoothed via  s m o o t h  for
c  various values of the control parameter  s .
      integer i,is,j,l,lsm,ndigit,npoint,ns
      real a(npoint,4),coefsm(4,npoint-1),dy(npoint)
     *    ,s, scrtch(npoint,7),sfp,x(npoint),y(npoint), smY(npoint)
c      equivalence (scrtch,coefsm)

         sfp = smooth ( x, y, dy, npoint, s, scrtch, a )
         do 10 i=1,npoint
  		smY(i)=a(i,1)
   10	print *, smY(i)
				return
         lsm = npoint - 1
         do 16 i=1,lsm
            do 16 j=1,4
   16          coefsm(j,i) = a(i,j)
         do 20 i=1,npoint
c            do 18 j=1,4
c   18          a(i,j) = ppvalu(x,coefsm,lsm,4,x(i),j-1)
   20		smY(i)=ppvalu(x,coefsm,lsm,4,x(i),0)
c   20    print 620, s(is), sfp, (i,(a(i,j),j=1,4),i=1,npoint,5)
c  620 format(15h prescribed s =,e10.3,23h, s(smoothing spline) =,e10.3/
c     *       54h value and derivatives of smoothing spline at corresp.
c     *      ,7h points/(i4,4e15.7))
c                                        stop
      end
