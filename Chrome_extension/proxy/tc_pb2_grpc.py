# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
"""Client and server classes corresponding to protobuf-defined services."""
import grpc

import tc_pb2 as tc__pb2


class towncrierStub(object):
    """Missing associated documentation comment in .proto file."""

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.attest = channel.unary_unary(
                '/rpc.towncrier/attest',
                request_serializer=tc__pb2.Empty.SerializeToString,
                response_deserializer=tc__pb2.Attestation.FromString,
                )
        self.status = channel.unary_unary(
                '/rpc.towncrier/status',
                request_serializer=tc__pb2.Empty.SerializeToString,
                response_deserializer=tc__pb2.Status.FromString,
                )
        self.process = channel.unary_unary(
                '/rpc.towncrier/process',
                request_serializer=tc__pb2.Request.SerializeToString,
                response_deserializer=tc__pb2.Response.FromString,
                )
        self.participate = channel.unary_unary(
                '/rpc.towncrier/participate',
                request_serializer=tc__pb2.Data.SerializeToString,
                response_deserializer=tc__pb2.Empty.FromString,
                )


class towncrierServicer(object):
    """Missing associated documentation comment in .proto file."""

    def attest(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def status(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def process(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def participate(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_towncrierServicer_to_server(servicer, server):
    rpc_method_handlers = {
            'attest': grpc.unary_unary_rpc_method_handler(
                    servicer.attest,
                    request_deserializer=tc__pb2.Empty.FromString,
                    response_serializer=tc__pb2.Attestation.SerializeToString,
            ),
            'status': grpc.unary_unary_rpc_method_handler(
                    servicer.status,
                    request_deserializer=tc__pb2.Empty.FromString,
                    response_serializer=tc__pb2.Status.SerializeToString,
            ),
            'process': grpc.unary_unary_rpc_method_handler(
                    servicer.process,
                    request_deserializer=tc__pb2.Request.FromString,
                    response_serializer=tc__pb2.Response.SerializeToString,
            ),
            'participate': grpc.unary_unary_rpc_method_handler(
                    servicer.participate,
                    request_deserializer=tc__pb2.Data.FromString,
                    response_serializer=tc__pb2.Empty.SerializeToString,
            ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
            'rpc.towncrier', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))


 # This class is part of an EXPERIMENTAL API.
class towncrier(object):
    """Missing associated documentation comment in .proto file."""

    @staticmethod
    def attest(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/rpc.towncrier/attest',
            tc__pb2.Empty.SerializeToString,
            tc__pb2.Attestation.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def status(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/rpc.towncrier/status',
            tc__pb2.Empty.SerializeToString,
            tc__pb2.Status.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def process(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/rpc.towncrier/process',
            tc__pb2.Request.SerializeToString,
            tc__pb2.Response.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def participate(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/rpc.towncrier/participate',
            tc__pb2.Data.SerializeToString,
            tc__pb2.Empty.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)
