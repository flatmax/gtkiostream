      subroutine colpnt(k,rho)

c*********************************************************************72
c
cc COLPNT supplies collocation points.
c
c  from  * a practical guide to splines *  by c. de boor    
c  the  k collocation points for the standard interval (-1,1) are sup-
c  plied here as the zeros of the legendre polynomial of degree k ,
c  provided  k .le. 8 . otherwise, uniformly spaced points are given.
c
      implicit none

      integer k,  j
      double precision rho(k),  fkm1o2
      if (k .gt. 8)                    go to 99
                                       go to (10,20,30,40,50,60,70,80),k
   10 rho(1) = 0.0D+00
                                       return
   20 rho(2) = .577350269189626D+00
      rho(1) = - rho(2)
                                       return
   30 rho(3) = .774596669241483D+00
      rho(1) = - rho(3)
      rho(2) = 0.0D+00
                                       return
   40 rho(3) = .339981043584856D+00
      rho(2) = - rho(3)
      rho(4) = .861136311594053D+00
      rho(1) = - rho(4)
                                       return
   50 rho(4) = .538469310105683D+00
      rho(2) = - rho(4)
      rho(5) = .906179845938664D+00
      rho(1) = - rho(5)
      rho(3) = 0.0D+00
                                       return
   60 rho(4) = .238619186083197D+00
      rho(3) = - rho(4)
      rho(5) = .66120 9386466265D+00
      rho(2) = - rho(5)
      rho(6) = .93246 9514203152D+00
      rho(1) = - rho(6)
                                       return
   70 rho(5) = .405845151377397D+00
      rho(3) = - rho(5)
      rho(6) = .741531185599394D+00
      rho(2) = - rho(6)
      rho(7) = .949107912342759D+00
      rho(1) = - rho(7)
      rho(4) = 0.0D+00
                                       return
   80 rho(5) = .183434642495650D+00
      rho(4) = - rho(5)
      rho(6) = .525532409916329D+00
      rho(3) = - rho(6)
      rho(7) = .796666477413627D+00
      rho(2) = - rho(7)
      rho(8) = .960289856497536D+00
      rho(1) = - rho(8)
                                       return
c  if k .gt. 8, use equispaced points, but print warning
   99 print 699,k
  699 format(11h **********/
     &       49h equispaced collocation points are used since k =,i2,
     &       19h is greater than 8.)
      fkm1o2 = dble(k-1)/2.0D+00
      do 100 j=1,k
  100    rho(j) = dble(j-1)/fkm1o2 - 1.0D+00
                                       return
      end
