from django.urls import path
from django.conf.urls import url,include
from . import views

urlpatterns=[
    path('',views.home,name='home'),
    path('about',views.about,name='about'),
    path('information',views.information,name='information'),
    path('hebdo',views.hebdo,name='hebdo'),
    path('daily',views.daily,name='daily'),
]