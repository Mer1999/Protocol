#pragma once
#include<iostream>
using namespace std;
#define MAX_PKT 1024

typedef enum {
	FALSE,
	TRUE
} boolean;

typedef unsigned int seq_nr;

/*���ݰ���������*/
typedef struct {
	unsigned char data[MAX_PKT];
} packet; 

 /*֡����ö����*/
typedef enum {
	data, //���ݰ� 
	ack, //ȷ�ϰ� 
	nak //��ȷ�ϰ�
} frame_kind;

/*֡�ṹ*/
typedef struct {
	frame_kind kind;	//֡���� 
	seq_nr seq;			//������� 
	seq_nr ack;			//������� 
	packet info;			//���ݰ�
} frame; 

/*�¼�����ö����*/
typedef enum {
	frame_arrival,				//֡����
	cksum_err,					//����ʹ�
	timeout,						//���ͳ�ʱ
	network_layer_ready,	//�������� 
	ack_timeout					//ȷ�ϰ���ʱ
} event_type; 

void wait_for_event(event_type* event);				//�����������ȴ��¼�����
void from_network_layer(packet* p);					//���ͷ��������õ������ݰ�
void to_network_layer(packet* p);						//���շ�������㷢�ʹ����ݰ� 
																			//ȥ��֡�����͡�����/ȷ����ŵȿ�����Ϣ
void from_physical_layer(packet* p);					//���շ��������ȡ��֡
																			//֡ͷβ��FLAG�ֽڡ������е��ֽ�������ȥ�� 
																			//���ñ�����ǰ����֤��У��ͣ����������� ����cksum_err�¼������ֻ��֡��ȷ�� ����»���ñ�����
void to_physical_layer(packet* p);						//���ͷ�������㷢��֡
																			//֡ͷβ��FLAG�ֽڡ������н����ֽ���� 
																			//����У��ͷ���֡β
void start_timer(seq_nr k);									//������k֡�Ķ�ʱ��
void stop_timer(seq_nr k);									//ֹͣ��k֡�Ķ�ʱ��
void start_ack_timer(void);									//����ȷ�ϰ���ʱ��
void stop_ack_timer(void);									//ֹͣȷ�ϰ���ʱ��
void enable_network_layer(void);						//������������
																			//ʹ���Բ����µ�network_layer_ready�¼�
void disable_network_layer(void);						//ʹ���������
																			//���ٲ����µ�network_layer_ready�¼�

#define inc(k) if(k<MAX_SEQ) k=k+1; else k=0; //ʹk��[0 ~ MAX_SEQ-1]��ѭ������ 
																			//���MAX_SEQ=1����0/1����