from django.conf import settings
from django.db import models
from django.utils import timezone

# Create your models here.
class device_update(models.Model):
    device_id=models.CharField(max_length=200)
    creation_date=models.DateTimeField(default=timezone.now)
    occupancy=models.IntegerField(max_length=200)
    usages=models.IntegerField(max_length=200)
    inevents=models.IntegerField(max_length=200)
    incleanevents=models.IntegerField(max_length=200)

