      subroutine titand ( tau, gtau, n )

c*********************************************************************72
c
cc TITAND represents a temperature dependent property of titanium.
c
c  from  * a practical guide to splines *  by c. de boor    
c  these data represent a property of titanium as a function of
c  temperature. they have been used extensively as an example in spline
c  approximation with variable knots.
c
      implicit none

      integer n,   i
      double precision gtau(49),tau(49),gtitan(49)
      data gtitan /
     &  0.644D+00, 0.622D+00, 0.638D+00, 0.649D+00, 0.652D+00,
     &  0.639D+00, 0.646D+00, 0.657D+00, 0.652D+00, 0.655D+00,
     &  0.644D+00, 0.663D+00, 0.663D+00, 0.668D+00, 0.676D+00,
     &  0.676D+00, 0.686D+00, 0.679D+00, 0.678D+00, 0.683D+00,
     &  0.694D+00, 0.699D+00, 0.710D+00, 0.730D+00, 0.763D+00,
     &  0.812D+00, 0.907D+00, 1.044D+00, 1.336D+00, 1.881D+00,
     &  2.169D+00, 2.075D+00, 1.598D+00, 1.211D+00, 0.916D+00,
     &  0.746D+00, 0.672D+00, 0.627D+00, 0.615D+00, 0.607D+00,
     &  0.606D+00, 0.609D+00, 0.603D+00, 0.601D+00, 0.603D+00,
     &  0.601D+00, 0.611D+00, 0.601D+00, 0.608D+00 /
      n = 49
      do 10 i=1,n
         tau(i) = 585.0D+00 + 10.0D+00*dble(i)
   10    gtau(i) = gtitan(i)
                                        return
      end
