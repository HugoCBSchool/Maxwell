from django.shortcuts import render,get_object_or_404
from .models import device_update
import string
import random
from django.db.models import Avg,Sum,F,RowRange,Window
from django.db.models.functions import datetime
from datetime import timedelta

def get_percent_sums_day():
    time_thresh=datetime.datetime.now()-timedelta(days=1)
    result = device_update.objects.filter(
        creation_date__gte=time_thresh
    ).order_by('creation_date')
    result_percent=result.aggregate(
        clean_percent=Sum('incleanevents')*100.0/(Sum('inevents')*1.0)
    )
    result_clean_list=result.values(
        'device_id'
    ).annotate(
        sum_clean=Sum('incleanevents'),
        sum_event=Sum('inevents')
    ).order_by()
    return result_percent,result_clean_list

def get_percent_sums_week():
    time_thresh = datetime.datetime.now() - timedelta(days=7)
    result = device_update.objects.filter(
        creation_date__gte=time_thresh
    ).order_by('creation_date')
    result_percent = result.aggregate(
        clean_percent=Sum('incleanevents') * 100.0 / (Sum('inevents') * 1.0)
    )
    result_clean_list = result.values(
        'device_id'
    ).annotate(
        sum_clean=Sum('incleanevent'),
        sum_event=Sum('inevent')
    ).order_by()
    return result_percent, result_clean_list


def id_generator(size=4, chars=string.digits):
    return 'B-'.join(random.choice(chars) for _ in range(size))


def home(request):
    return render(request, 'main/home.html', {})

def information(request):
    random.seed()
    time_thresh = datetime.datetime.now() - timedelta(days=30)
    result = device_update.objects.filter(
        creation_date__gte=time_thresh
    ).order_by('-creation_date')

    classid_list = [id_generator() for i in range(6)]
    classls = [
        {'class_name': id_generator()
            , 'current_nb': random.randint(0, 25)
            , 'total_nb': random.randint(0, 150)
         } for i in range(6)
    ]
    classid_list.append(result[0].device_id)
    classls.append(
        {'class_name': result[0].device_id
            , 'current_nb': result[0].occupancy
            , 'total_nb': result[0].occupancy
         }
    )
    return render(request, 'main/information.html', {
        'classrooms':classls,
        'qt_desinfectant':result[0].usages
    })

def hebdo(request):
    random.seed()
    classid_list=[id_generator() for i in range(6)]
    classls=[
        {       'class_name':id_generator()
            ,   'current_nb':random.randint(0,25)
            ,   'total_nb':random.randint(0,150)
        }   for i in range(6)
    ]
    perc, ls = get_percent_sums_day()
    for i in ls:
        classid_list.append(i['device_id'])
        classls.append({     'class_name': i['device_id']
                , 'current_nb': i['sum_clean']
                , 'total_nb': i['sum_event']
            })
    return render(request, 'main/hebdo.html', {
            'classrooms':classls
        ,   'class_percent':perc['clean_percent']
    })

def daily(request):
    random.seed()
    perc,ls=get_percent_sums_day()
    classid_list=[id_generator() for i in range(6)]
    classls = [
        {'class_name': id_generator()
            , 'current_nb': random.randint(0, 25)
            , 'total_nb': random.randint(0, 150)
         } for i in range(6)
    ]
    for i in ls:
        classid_list.append(i['device_id'])
        classls.append({     'class_name': i['device_id']
                , 'current_nb': i['sum_clean']
                , 'total_nb': i['sum_event']
            })


    return render(request, 'main/daily.html', {
            'classrooms':classls
        ,   'class_percent':perc['clean_percent']
    })

def about(request):
    return render(request, 'main/about.html', {})